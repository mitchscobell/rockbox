/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2005 Miika Pekkarinen
 * Copyright (C) 2012 Michael Sevakis
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/
#include "config.h"
#include "system.h"
#include "fracmul.h"
#include "fixedpoint.h"
#include "dsp_proc_entry.h"
#include <string.h>

/**
 * Linear interpolation resampling that introduces a one sample delay because
 * of our inability to look into the future at the end of a frame.
 */

#if 0 /* Set to '1' to enable debug messages */
#include <debug.h>
#else
#undef DEBUGF
#define DEBUGF(...)
#endif

#define RESAMPLE_BUF_COUNT 192 /* Per channel, per DSP */

/* CODEC_IDX_AUDIO = left and right, CODEC_IDX_VOICE = mono */
static int32_t resample_out_bufs[3][RESAMPLE_BUF_COUNT] IBSS_ATTR;

/* Data for each resampler on each DSP */
static struct resample_data
{
    uint32_t delta;          /* 00h: Phase delta for each step */
    uint32_t phase;          /* 04h: Current phase [pos16|frac16] */
    int32_t  last_sample[2]; /* 08h: Last samples for interpolation (L+R) */
                             /* 10h */
    int32_t  frequency;      /* Virtual samplerate */
    struct dsp_buffer resample_buf; /* Buffer descriptor for resampled data */
    int32_t *resample_out_p[2];     /* Actual output buffer pointers */
} resample_data[DSP_COUNT] IBSS_ATTR;

/* Actual worker function. Implemented here or in target assembly code. */
int lin_resample_resample(struct resample_data *data, struct dsp_buffer *src,
                          struct dsp_buffer *dst);

static void lin_resample_flush_data(struct resample_data *data)
{
    data->phase = 0;
    data->last_sample[0] = 0;
    data->last_sample[1] = 0;
}

static void lin_resample_flush(struct dsp_proc_entry *this)
{
    struct resample_data *data = (void *)this->data;
    data->resample_buf.remcount = 0;
    lin_resample_flush_data(data);
}

static bool lin_resample_new_delta(struct resample_data *data,
                                   struct sample_format *format)
{
    int32_t frequency = format->frequency; /* virtual samplerate */

    data->frequency = frequency;
    data->delta = fp_div(frequency, NATIVE_FREQUENCY, 16);

    if (frequency == NATIVE_FREQUENCY)
    {
        /* NOTE: If fully glitch-free transistions from no resampling to
           resampling are desired, last_sample history should be maintained
           even when not resampling. */
        lin_resample_flush_data(data);
        return false;
    }

    return true;
}

#if !defined(CPU_COLDFIRE) && !defined(CPU_ARM)
/* Where the real work is done */
int lin_resample_resample(struct resample_data *data, struct dsp_buffer *src,
                          struct dsp_buffer *dst)
{
    int ch = src->format.num_channels - 1;
    uint32_t count = MIN(src->remcount, 0x8000);
    uint32_t delta = data->delta;
    uint32_t phase, pos;
    int32_t *d;

    do
    {
        const int32_t *s = src->p32[ch];

        d = dst->p32[ch];
        int32_t *dmax = d + dst->bufcount;

        phase = data->phase;
        pos = phase >> 16;
        pos = MIN(pos, count);

        int32_t last = pos > 0 ? s[pos - 1] : data->last_sample[ch];

        if (pos < count)
        {
            while (1)
            {
                *d++ = last + FRACMUL((phase & 0xffff) << 15, s[pos] - last);
                phase += delta;
                pos = phase >> 16;

                if (pos >= count || d >= dmax)
                    break;

                if (pos > 0)
                    last = s[pos - 1];
            }

            if (pos > 0)
            {
                pos = MIN(pos, count);
                last = s[pos - 1];
            }
        }

        data->last_sample[ch] = last;
    }
    while (--ch >= 0);

    /* Wrap phase accumulator back to start of next frame. */
    data->phase = phase - (pos << 16);

    dst->remcount = d - dst->p32[0];

    return pos;
}
#endif /* CPU */

/* Resample count stereo samples or stop when the destination is full.
 * Updates the src buffer and changes to its own output buffer to refer to
 * the resampled data. */
static void lin_resample_process(struct dsp_proc_entry *this,
                                 struct dsp_buffer **buf_p)
{
    struct resample_data *data = (void *)this->data;
    struct dsp_buffer *src = *buf_p;
    struct dsp_buffer *dst = &data->resample_buf;

    *buf_p = dst;

    if (dst->remcount > 0)
        return; /* data still remains */

    dst->remcount = 0;
    dst->p32[0] = data->resample_out_p[0];
    dst->p32[1] = data->resample_out_p[1];

    if (src->remcount > 0)
    {
        dst->bufcount = RESAMPLE_BUF_COUNT;

        int consumed = lin_resample_resample(data, src, dst);

        /* Advance src by consumed amount */
        if (consumed > 0)
            dsp_advance_buffer32(src, consumed);
    }
    /* else purged resample_buf */

    /* Inherit in-place processed mask from source buffer */
    dst->proc_mask = src->proc_mask;
}

/* Finish draining old samples then switch format or shut off */
static intptr_t lin_resample_new_format(struct dsp_proc_entry *this,
                                        struct dsp_config *dsp,
                                        struct sample_format *format)
{
    struct resample_data *data = (void *)this->data;
    struct dsp_buffer *dst = &data->resample_buf;

    if (dst->remcount > 0)
        return PROC_NEW_FORMAT_TRANSITION;

    DSP_PRINT_FORMAT(DSP_PROC_RESAMPLE, *format);

    int32_t frequency = data->frequency;
    bool active = dsp_proc_active(dsp, DSP_PROC_RESAMPLE);

    if (format->frequency != frequency)
    {
        DEBUGF("  DSP_PROC_RESAMPLE- new delta\n");
        active = lin_resample_new_delta(data, format);
        dsp_proc_activate(dsp, DSP_PROC_RESAMPLE, active);
    }

    /* Everything after us is NATIVE_FREQUENCY */
    dst->format = *format;
    dst->format.frequency = NATIVE_FREQUENCY;
    dst->format.codec_frequency = NATIVE_FREQUENCY;

    if (active)
        return PROC_NEW_FORMAT_OK;

    /* No longer needed */
    DEBUGF("  DSP_PROC_RESAMPLE- deactivated\n");
    return PROC_NEW_FORMAT_DEACTIVATED;
}

static void INIT_ATTR lin_resample_dsp_init(struct dsp_config *dsp,
                                            enum dsp_ids dsp_id)
{
    int32_t *lbuf, *rbuf;

    switch (dsp_id)
    {
    case CODEC_IDX_AUDIO:
        lbuf = resample_out_bufs[0];
        rbuf = resample_out_bufs[1];
        break;

    case CODEC_IDX_VOICE:
        lbuf = rbuf = resample_out_bufs[2]; /* Always mono */
        break;

    default:
        /* huh? */
        DEBUGF("DSP_PROC_RESAMPLE- unknown DSP %d\n", (int)dsp_id);
        return;
    }

    /* Always enable resampler so that format changes may be monitored and
     * it self-activated when required */
    dsp_proc_enable(dsp, DSP_PROC_RESAMPLE, true);
    resample_data[dsp_id].resample_out_p[0] = lbuf;
    resample_data[dsp_id].resample_out_p[1] = rbuf;
}

static void INIT_ATTR lin_resample_proc_init(struct dsp_proc_entry *this,
                                             struct dsp_config *dsp)
{
    dsp_proc_set_in_place(dsp, DSP_PROC_RESAMPLE, false);
    this->data = (intptr_t)&resample_data[dsp_get_id(dsp)];
    this->process = lin_resample_process;
}

/* DSP message hook */
static intptr_t lin_resample_configure(struct dsp_proc_entry *this,
                                       struct dsp_config *dsp,
                                       unsigned int setting,
                                       intptr_t value)
{
    intptr_t retval = 0;

    switch (setting)
    {
    case DSP_INIT:
        lin_resample_dsp_init(dsp, (enum dsp_ids)value);
        break;

    case DSP_FLUSH:
        lin_resample_flush(this);
        break;

    case DSP_PROC_INIT:
        lin_resample_proc_init(this, dsp);
        break;

    case DSP_PROC_CLOSE:
        /* This stage should be enabled at all times */
        DEBUGF("DSP_PROC_RESAMPLE- Error: Closing!\n");
        break;

    case DSP_PROC_NEW_FORMAT:
        retval = lin_resample_new_format(this, dsp,
                                         (struct sample_format *)value);
        break;
    }

    return retval;
}

/* Database entry */
DSP_PROC_DB_ENTRY(RESAMPLE,
                  lin_resample_configure);
