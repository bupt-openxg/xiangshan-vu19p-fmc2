#ifndef FFT_ROI_PMU_H
#define FFT_ROI_PMU_H

#include <csr.h>
#include <stdint.h>

#ifndef FFT_ROI_HPM_ENABLE
#define FFT_ROI_HPM_ENABLE 1
#endif

#define FFT_ROI_HPM_PROFILE_CANONICAL 0
#define FFT_ROI_HPM_PROFILE_DIAGNOSTIC 1
#define FFT_ROI_HPM_PROFILE_BACKEND_DIAG 2

#ifndef FFT_ROI_HPM_PROFILE
#define FFT_ROI_HPM_PROFILE FFT_ROI_HPM_PROFILE_CANONICAL
#endif

typedef struct {
    const char *group;
    const char *event;
    uint8_t counter_id;
    uint16_t selector;
} fft_roi_hpm_event_desc_t;

typedef struct {
    uint64_t inhibit_mask;
    uint64_t values[12];
} fft_roi_hpm_state_t;

#define FFT_ROI_HPM_MAX_EVENT_COUNT 12
#define FFT_ROI_HPM_MODE_M 0x10ULL
#define FFT_ROI_HPM_MODE_OFFSET 59

/*
 * Selector indices are derived from the current XiangShan source tree /
 * generated event-coding logs:
 * - frontend allPerfEvents =
 *   noEvent + IFU + IBuffer + ICache + Ftq + BPU
 *   - IFU `ifu_miss` = 3
 *   - IBuffer `IBuffer_full` = 20
 * - backend allPerfEvents =
 *   noEvent + CtrlBlock(decode + rename + dispatch + rob) + dataPath + ...
 *   - Decode `decoder_stall_cycle` = 3
 *   - ROB `rob_commitInstrLoad` = 53
 *   - ROB `rob_commitInstrStore` = 55
 *   - ROB `rob_commitInstrBranch` = 54
 *   - ROB `BR_MIS_PRED` = 63
 *   - TopDown `EXEC_STALL_CYCLE` = 65
 *   - TopDown `MEMSTALL_STORE/L1MISS/L2MISS/L3MISS` = 66/67/68/69
 */
static const fft_roi_hpm_event_desc_t k_fft_roi_hpm_events_canonical[] = {
    {"frontend", "ifu_miss", 3, 3},
    {"frontend", "IBuffer_full", 4, 20},
    {"backend", "decoder_stall_cycle", 11, 3},
    {"backend", "rob_commitInstrLoad", 12, 53},
    {"backend", "rob_commitInstrStore", 13, 55},
};

static const fft_roi_hpm_event_desc_t k_fft_roi_hpm_events_diagnostic[] = {
    {"frontend", "ifu_miss", 3, 3},
    {"frontend", "IBuffer_full", 4, 20},
    {"backend", "decoder_stall_cycle", 11, 3},
    {"backend", "rob_commitInstrLoad", 12, 53},
    {"backend", "rob_commitInstrBranch", 13, 54},
    {"backend", "rob_commitInstrStore", 14, 55},
    {"backend", "BR_MIS_PRED", 15, 63},
    {"backend", "EXEC_STALL_CYCLE", 16, 65},
    {"backend", "MEMSTALL_STORE", 17, 66},
    {"backend", "MEMSTALL_L1MISS", 18, 67},
    {"backend", "MEMSTALL_L2MISS", 19, 68},
    {"backend", "MEMSTALL_L3MISS", 20, 69},
};

/*
 * Backend-only diagnostic profile for ROI attribution.
 *
 * Unlike the legacy `diagnostic` profile above, this set stays strictly
 * inside backend slots `mhpmevent11..18` so it can be used as a safer
 * diagnose-mode companion to the canonical publish profile without mixing
 * backend selectors into memblock counter IDs.
 */
static const fft_roi_hpm_event_desc_t k_fft_roi_hpm_events_backend_diag[] = {
    {"backend", "decoder_stall_cycle", 11, 3},
    {"backend", "rob_commitInstrLoad", 12, 53},
    {"backend", "rob_commitInstrBranch", 13, 54},
    {"backend", "rob_commitInstrStore", 14, 55},
    {"backend", "BR_MIS_PRED", 15, 63},
    {"backend", "EXEC_STALL_CYCLE", 16, 65},
    {"backend", "MEMSTALL_STORE", 17, 66},
    {"backend", "MEMSTALL_L1MISS", 18, 67},
};

static inline uint32_t fft_roi_hpm_event_count(void)
{
#if FFT_ROI_HPM_ENABLE
#if FFT_ROI_HPM_PROFILE == FFT_ROI_HPM_PROFILE_CANONICAL
    return (uint32_t)(sizeof(k_fft_roi_hpm_events_canonical) / sizeof(k_fft_roi_hpm_events_canonical[0]));
#elif FFT_ROI_HPM_PROFILE == FFT_ROI_HPM_PROFILE_DIAGNOSTIC
    return (uint32_t)(sizeof(k_fft_roi_hpm_events_diagnostic) / sizeof(k_fft_roi_hpm_events_diagnostic[0]));
#elif FFT_ROI_HPM_PROFILE == FFT_ROI_HPM_PROFILE_BACKEND_DIAG
    return (uint32_t)(sizeof(k_fft_roi_hpm_events_backend_diag) / sizeof(k_fft_roi_hpm_events_backend_diag[0]));
#else
#error "Unsupported FFT_ROI_HPM_PROFILE"
#endif
#else
    return 0;
#endif
}

static inline const fft_roi_hpm_event_desc_t *fft_roi_hpm_event_desc(uint32_t index)
{
#if FFT_ROI_HPM_PROFILE == FFT_ROI_HPM_PROFILE_CANONICAL
    return &k_fft_roi_hpm_events_canonical[index];
#elif FFT_ROI_HPM_PROFILE == FFT_ROI_HPM_PROFILE_DIAGNOSTIC
    return &k_fft_roi_hpm_events_diagnostic[index];
#elif FFT_ROI_HPM_PROFILE == FFT_ROI_HPM_PROFILE_BACKEND_DIAG
    return &k_fft_roi_hpm_events_backend_diag[index];
#else
#error "Unsupported FFT_ROI_HPM_PROFILE"
#endif
}

static inline uint64_t fft_roi_hpm_single_selector(uint16_t selector)
{
    return (FFT_ROI_HPM_MODE_M << FFT_ROI_HPM_MODE_OFFSET) | (uint64_t)selector;
}

static inline void fft_roi_write_mhpmevent(uint8_t counter_id, uint64_t value)
{
    switch (counter_id) {
    case 3: csr_write(mhpmevent3, value); return;
    case 4: csr_write(mhpmevent4, value); return;
    case 5: csr_write(mhpmevent5, value); return;
    case 6: csr_write(mhpmevent6, value); return;
    case 7: csr_write(mhpmevent7, value); return;
    case 8: csr_write(mhpmevent8, value); return;
    case 9: csr_write(mhpmevent9, value); return;
    case 10: csr_write(mhpmevent10, value); return;
    case 11: csr_write(mhpmevent11, value); return;
    case 12: csr_write(mhpmevent12, value); return;
    case 13: csr_write(mhpmevent13, value); return;
    case 14: csr_write(mhpmevent14, value); return;
    case 15: csr_write(mhpmevent15, value); return;
    case 16: csr_write(mhpmevent16, value); return;
    case 17: csr_write(mhpmevent17, value); return;
    case 18: csr_write(mhpmevent18, value); return;
    case 19: csr_write(mhpmevent19, value); return;
    case 20: csr_write(mhpmevent20, value); return;
    case 21: csr_write(mhpmevent21, value); return;
    case 22: csr_write(mhpmevent22, value); return;
    case 23: csr_write(mhpmevent23, value); return;
    case 24: csr_write(mhpmevent24, value); return;
    case 25: csr_write(mhpmevent25, value); return;
    case 26: csr_write(mhpmevent26, value); return;
    case 27: csr_write(mhpmevent27, value); return;
    case 28: csr_write(mhpmevent28, value); return;
    case 29: csr_write(mhpmevent29, value); return;
    case 30: csr_write(mhpmevent30, value); return;
    case 31: csr_write(mhpmevent31, value); return;
    default: return;
    }
}

static inline void fft_roi_write_mhpmcounter(uint8_t counter_id, uint64_t value)
{
    switch (counter_id) {
    case 3: csr_write(mhpmcounter3, value); return;
    case 4: csr_write(mhpmcounter4, value); return;
    case 5: csr_write(mhpmcounter5, value); return;
    case 6: csr_write(mhpmcounter6, value); return;
    case 7: csr_write(mhpmcounter7, value); return;
    case 8: csr_write(mhpmcounter8, value); return;
    case 9: csr_write(mhpmcounter9, value); return;
    case 10: csr_write(mhpmcounter10, value); return;
    case 11: csr_write(mhpmcounter11, value); return;
    case 12: csr_write(mhpmcounter12, value); return;
    case 13: csr_write(mhpmcounter13, value); return;
    case 14: csr_write(mhpmcounter14, value); return;
    case 15: csr_write(mhpmcounter15, value); return;
    case 16: csr_write(mhpmcounter16, value); return;
    case 17: csr_write(mhpmcounter17, value); return;
    case 18: csr_write(mhpmcounter18, value); return;
    case 19: csr_write(mhpmcounter19, value); return;
    case 20: csr_write(mhpmcounter20, value); return;
    case 21: csr_write(mhpmcounter21, value); return;
    case 22: csr_write(mhpmcounter22, value); return;
    case 23: csr_write(mhpmcounter23, value); return;
    case 24: csr_write(mhpmcounter24, value); return;
    case 25: csr_write(mhpmcounter25, value); return;
    case 26: csr_write(mhpmcounter26, value); return;
    case 27: csr_write(mhpmcounter27, value); return;
    case 28: csr_write(mhpmcounter28, value); return;
    case 29: csr_write(mhpmcounter29, value); return;
    case 30: csr_write(mhpmcounter30, value); return;
    case 31: csr_write(mhpmcounter31, value); return;
    default: return;
    }
}

static inline uint64_t fft_roi_read_mhpmcounter(uint8_t counter_id)
{
    switch (counter_id) {
    case 3: return csr_read(mhpmcounter3);
    case 4: return csr_read(mhpmcounter4);
    case 5: return csr_read(mhpmcounter5);
    case 6: return csr_read(mhpmcounter6);
    case 7: return csr_read(mhpmcounter7);
    case 8: return csr_read(mhpmcounter8);
    case 9: return csr_read(mhpmcounter9);
    case 10: return csr_read(mhpmcounter10);
    case 11: return csr_read(mhpmcounter11);
    case 12: return csr_read(mhpmcounter12);
    case 13: return csr_read(mhpmcounter13);
    case 14: return csr_read(mhpmcounter14);
    case 15: return csr_read(mhpmcounter15);
    case 16: return csr_read(mhpmcounter16);
    case 17: return csr_read(mhpmcounter17);
    case 18: return csr_read(mhpmcounter18);
    case 19: return csr_read(mhpmcounter19);
    case 20: return csr_read(mhpmcounter20);
    case 21: return csr_read(mhpmcounter21);
    case 22: return csr_read(mhpmcounter22);
    case 23: return csr_read(mhpmcounter23);
    case 24: return csr_read(mhpmcounter24);
    case 25: return csr_read(mhpmcounter25);
    case 26: return csr_read(mhpmcounter26);
    case 27: return csr_read(mhpmcounter27);
    case 28: return csr_read(mhpmcounter28);
    case 29: return csr_read(mhpmcounter29);
    case 30: return csr_read(mhpmcounter30);
    case 31: return csr_read(mhpmcounter31);
    default: return 0;
    }
}

static inline void fft_roi_hpm_reset(fft_roi_hpm_state_t *state)
{
    state->inhibit_mask = 0;
    for (uint32_t i = 0; i < FFT_ROI_HPM_MAX_EVENT_COUNT; ++i) {
        state->values[i] = 0;
    }
}

static inline void fft_roi_hpm_prepare(fft_roi_hpm_state_t *state)
{
    fft_roi_hpm_reset(state);

#if FFT_ROI_HPM_ENABLE
    const uint32_t count = fft_roi_hpm_event_count();
    for (uint32_t i = 0; i < count; ++i) {
        state->inhibit_mask |= (1ULL << fft_roi_hpm_event_desc(i)->counter_id);
    }

    if (state->inhibit_mask != 0) {
        csr_set(mcountinhibit, state->inhibit_mask);
    }

    asm volatile("" ::: "memory");
    for (uint32_t i = 0; i < count; ++i) {
        const fft_roi_hpm_event_desc_t *desc = fft_roi_hpm_event_desc(i);
        fft_roi_write_mhpmevent(desc->counter_id, fft_roi_hpm_single_selector(desc->selector));
        fft_roi_write_mhpmcounter(desc->counter_id, 0);
    }
#endif
}

static inline void fft_roi_hpm_begin(const fft_roi_hpm_state_t *state)
{
#if FFT_ROI_HPM_ENABLE
    asm volatile("" ::: "memory");
    if (state->inhibit_mask != 0) {
        csr_clear(mcountinhibit, state->inhibit_mask);
    }
#else
    (void)state;
#endif
}

static inline void fft_roi_hpm_end(fft_roi_hpm_state_t *state)
{
#if FFT_ROI_HPM_ENABLE
    if (state->inhibit_mask == 0) {
        return;
    }

    csr_set(mcountinhibit, state->inhibit_mask);
    asm volatile("" ::: "memory");

    const uint32_t count = fft_roi_hpm_event_count();
    for (uint32_t i = 0; i < count; ++i) {
        state->values[i] = fft_roi_read_mhpmcounter(fft_roi_hpm_event_desc(i)->counter_id);
    }
#else
    (void)state;
#endif
}

#endif
