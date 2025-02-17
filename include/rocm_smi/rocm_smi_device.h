/*
 * =============================================================================
 * The University of Illinois/NCSA
 * Open Source License (NCSA)
 *
 * Copyright (c) 2017, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Developed by:
 *
 *                 AMD Research and AMD ROC Software Development
 *
 *                 Advanced Micro Devices, Inc.
 *
 *                 www.amd.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal with the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimers.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimers in
 *    the documentation and/or other materials provided with the distribution.
 *  - Neither the names of <Name of Development Group, Name of Institution>,
 *    nor the names of its contributors may be used to endorse or promote
 *    products derived from this Software without specific prior written
 *    permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS WITH THE SOFTWARE.
 *
 */
#ifndef INCLUDE_ROCM_SMI_ROCM_SMI_DEVICE_H_
#define INCLUDE_ROCM_SMI_ROCM_SMI_DEVICE_H_

#include <pthread.h>

#include <string>
#include <memory>
#include <utility>
#include <cstdint>
#include <vector>
#include <unordered_set>
#include <map>

#include "rocm_smi/rocm_smi_monitor.h"
#include "rocm_smi/rocm_smi_power_mon.h"
#include "rocm_smi/rocm_smi_common.h"
#include "rocm_smi/rocm_smi.h"
#include "rocm_smi/rocm_smi_counters.h"
#include "shared_mutex.h"   //NOLINT

namespace amd {
namespace smi {

enum DevKFDNodePropTypes {
  kDevKFDNodePropCachesCnt,
  kDevKFDNodePropIoLinksCnt,
  kDevKFDNodePropCPUCoreIdBase,
  kDevKFDNodePropSimdIdBase,
  kDevKFDNodePropMaxWavePerSimd,
  kDevKFDNodePropLdsSz,
  kDevKFDNodePropGdsSz,
  kDevKFDNodePropNumGWS,
  kDevKFDNodePropWaveFrontSize,
  kDevKFDNodePropArrCnt,
  kDevKFDNodePropSimdArrPerEng,
  kDevKFDNodePropCuPerSimdArr,
  kDevKFDNodePropSimdPerCU,
  kDevKFDNodePropMaxSlotsScratchCu,
  kDevKFDNodePropVendorId,
  kDevKFDNodePropDeviceId,
  kDevKFDNodePropLocationId,
  kDevKFDNodePropDrmRenderMinor,
  kDevKFDNodePropHiveId,
  kDevKFDNodePropNumSdmaEngines,
  kDevKFDNodePropNumSdmaXgmiEngs,
  kDevKFDNodePropMaxEngClkFComp,
  kDevKFDNodePropLocMemSz,
  kDevKFDNodePropFwVer,
  kDevKFDNodePropCapability,
  kDevKFDNodePropDbgProp,
  kDevKFDNodePropSdmaFwVer,
  kDevKFDNodePropMaxEngClkCComp,
  kDevKFDNodePropDomain,
};

enum DevInfoTypes {
  kDevPerfLevel,
  kDevOverDriveLevel,
  kDevMemOverDriveLevel,
  kDevDevID,
  kDevDevProdName,
  kDevDevProdNum,
  kDevVendorID,
  kDevSubSysDevID,
  kDevSubSysVendorID,
  kDevGPUMClk,
  kDevGPUSClk,
  kDevDCEFClk,
  kDevFClk,
  kDevSOCClk,
  kDevPCIEClk,
  kDevPowerProfileMode,
  kDevUsage,
  kDevPowerODVoltage,
  kDevVBiosVer,
  kDevPCIEThruPut,
  kDevErrCntSDMA,
  kDevErrCntUMC,
  kDevErrCntGFX,
  kDevErrCntMMHUB,
  kDevErrCntPCIEBIF,
  kDevErrCntHDP,
  kDevErrCntXGMIWAFL,
  kDevErrCntFeatures,
  kDevMemTotGTT,
  kDevMemTotVisVRAM,
  kDevMemTotVRAM,
  kDevMemUsedGTT,
  kDevMemUsedVisVRAM,
  kDevMemUsedVRAM,
  kDevVramVendor,
  kDevPCIEReplayCount,
  kDevUniqueId,
  kDevDFCountersAvailable,
  kDevMemBusyPercent,
  kDevXGMIError,
  kDevFwVersionAsd,
  kDevFwVersionCe,
  kDevFwVersionDmcu,
  kDevFwVersionMc,
  kDevFwVersionMe,
  kDevFwVersionMec,
  kDevFwVersionMec2,
  kDevFwVersionPfp,
  kDevFwVersionRlc,
  kDevFwVersionRlcSrlc,
  kDevFwVersionRlcSrlg,
  kDevFwVersionRlcSrls,
  kDevFwVersionSdma,
  kDevFwVersionSdma2,
  kDevFwVersionSmc,
  kDevFwVersionSos,
  kDevFwVersionTaRas,
  kDevFwVersionTaXgmi,
  kDevFwVersionUvd,
  kDevFwVersionVce,
  kDevFwVersionVcn,
  kDevSerialNumber,
  kDevMemPageBad,
  kDevNumaNode,
  kDevGpuMetrics,
  kDevGpuReset
};

typedef struct {
    std::vector<const char *> mandatory_depends;
    std::vector<DevInfoTypes> variants;
} dev_depends_t;

class Device {
 public:
    explicit Device(std::string path, RocmSMI_env_vars const *e);
    ~Device(void);

    void set_monitor(std::shared_ptr<Monitor> m) {monitor_ = m;}
    std::string path(void) const {return path_;}
    const std::shared_ptr<Monitor>& monitor() {return monitor_;}
    const std::shared_ptr<PowerMon>& power_monitor() {return power_monitor_;}
    void set_power_monitor(std::shared_ptr<PowerMon> pm) {power_monitor_ = pm;}

    int readDevInfo(DevInfoTypes type, uint64_t *val);
    int readDevInfoLine(DevInfoTypes type, std::string *line);
    int readDevInfo(DevInfoTypes type, std::string *val);
    int readDevInfo(DevInfoTypes type, std::vector<std::string> *retVec);
    int readDevInfo(DevInfoTypes type, std::size_t b_size,
                                      void *p_binary_data);
    int writeDevInfo(DevInfoTypes type, uint64_t val);
    int writeDevInfo(DevInfoTypes type, std::string val);

    uint32_t index(void) const {return card_indx_;}
    void set_card_index(uint32_t index) {card_indx_ = index;}
    uint32_t drm_render_minor(void) const {return drm_render_minor_;}
    void set_drm_render_minor(uint32_t minor) {drm_render_minor_ = minor;}
    static rsmi_dev_perf_level perfLvlStrToEnum(std::string s);
    uint64_t bdfid(void) const {return bdfid_;}
    void set_bdfid(uint64_t val) {bdfid_ = val;}
    pthread_mutex_t *mutex(void) {return mutex_.ptr;}
    evt::dev_evt_grp_set_t* supported_event_groups(void) {
                                             return &supported_event_groups_;}
    SupportedFuncMap *supported_funcs(void) {return &supported_funcs_;}
    uint64_t kfd_gpu_id(void) const {return kfd_gpu_id_;}
    void set_kfd_gpu_id(uint64_t id) {kfd_gpu_id_ = id;}

    void set_evt_notif_anon_file_ptr(FILE *f) {evt_notif_anon_file_ptr_ = f;}
    FILE *evt_notif_anon_file_ptr(void) const {return evt_notif_anon_file_ptr_;}
    void set_evt_notif_anon_fd(int fd) {evt_notif_anon_fd_ = fd;}
    void set_evt_notif_anon_fd(uint32_t fd) {
                                   evt_notif_anon_fd_ = static_cast<int>(fd);}
    int evt_notif_anon_fd(void) const {return evt_notif_anon_fd_;}
    metrics_table_header_t & gpu_metrics_ver(void) {return gpu_metrics_ver_;}
    void fillSupportedFuncs(void);
    void DumpSupportedFunctions(void);
    bool DeviceAPISupported(std::string name, uint64_t variant,
                                                        uint64_t sub_variant);

 private:
    std::shared_ptr<Monitor> monitor_;
    std::shared_ptr<PowerMon> power_monitor_;
    std::string path_;
    shared_mutex_t mutex_;
    uint32_t card_indx_;  // This index corresponds to the drm index (ie, card#)
    uint32_t drm_render_minor_;
    const RocmSMI_env_vars *env_;
    template <typename T> int openDebugFileStream(DevInfoTypes type, T *fs,
                                                   const char *str = nullptr);
    template <typename T> int openSysfsFileStream(DevInfoTypes type, T *fs,
                                                   const char *str = nullptr);
    int readDebugInfoStr(DevInfoTypes type, std::string *retStr);
    int readDevInfoStr(DevInfoTypes type, std::string *retStr);
    int readDevInfoMultiLineStr(DevInfoTypes type,
                                            std::vector<std::string> *retVec);
    int readDevInfoBinary(DevInfoTypes type, std::size_t b_size,
                                            void *p_binary_data);
    int writeDevInfoStr(DevInfoTypes type, std::string valStr);
    uint64_t bdfid_;
    uint64_t kfd_gpu_id_;
    std::unordered_set<rsmi_event_group_t,
                       evt::RSMIEventGrpHashFunction> supported_event_groups_;
    // std::map<std::string, uint64_t> kfdNodePropMap_;
    SupportedFuncMap supported_funcs_;

    int evt_notif_anon_fd_;
    FILE *evt_notif_anon_file_ptr_;
    struct metrics_table_header_t gpu_metrics_ver_;
};

}  // namespace smi
}  // namespace amd

#endif  // INCLUDE_ROCM_SMI_ROCM_SMI_DEVICE_H_
