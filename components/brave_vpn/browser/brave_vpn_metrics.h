/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_VPN_BROWSER_BRAVE_VPN_METRICS_H_
#define BRAVE_COMPONENTS_BRAVE_VPN_BROWSER_BRAVE_VPN_METRICS_H_

#include "base/memory/weak_ptr.h"
#include "base/timer/wall_clock_timer.h"
#include "brave/components/misc_metrics/uptime_monitor.h"
#include "brave/components/time_period_storage/weekly_storage.h"
#include "build/build_config.h"
#include "components/prefs/pref_change_registrar.h"

class PrefService;

namespace brave_vpn {

inline constexpr char kNewUserReturningHistogramName[] =
    "Brave.VPN.NewUserReturning";
inline constexpr char kDaysInMonthUsedHistogramName[] =
    "Brave.VPN.DaysInMonthUsed";
inline constexpr char kLastUsageTimeHistogramName[] = "Brave.VPN.LastUsageTime";
inline constexpr char kWidgetUsageHistogramName[] = "Brave.VPN.WidgetUsage";
inline constexpr char kHideWidgetHistogramName[] = "Brave.VPN.HideWidget";
inline constexpr char kVPNConnectedDurationHistogramName[] =
    "Brave.VPN.ConnectedDuration";

class BraveVpnMetrics {
 public:
  class Delegate {
   public:
    virtual ~Delegate() = default;

    virtual bool is_purchased_user() const = 0;

#if !BUILDFLAG(IS_ANDROID)
    virtual bool IsConnected() const = 0;
#endif
  };

  BraveVpnMetrics(PrefService* local_prefs,
                  PrefService* profile_prefs,
                  base::WeakPtr<misc_metrics::UptimeMonitor> uptime_monitor,
                  Delegate* delegate = nullptr);
  ~BraveVpnMetrics();

  BraveVpnMetrics(const BraveVpnMetrics&) = delete;
  BraveVpnMetrics& operator=(const BraveVpnMetrics&) = delete;

  // new_usage should be set to true if a new VPN connection was just
  // established.
  void RecordAllMetrics(bool new_usage);

#if BUILDFLAG(IS_ANDROID)
  void RecordAndroidBackgroundP3A(int64_t session_start_time_ms,
                                  int64_t session_end_time_ms);
#endif

  void RecordWidgetUsage(bool new_usage);

  // Records one minute of VPN connected time
  void RecordVPNConnectedInterval();

 private:
  void HandleShowWidgetChange();

  void ReportVPNConnectedDuration();

  raw_ptr<PrefService> local_state_;
  raw_ptr<PrefService> profile_prefs_;
  PrefChangeRegistrar pref_change_registrar_;
  base::WeakPtr<misc_metrics::UptimeMonitor> uptime_monitor_;
  raw_ptr<Delegate> delegate_ = nullptr;

  WeeklyStorage widget_usage_storage_;
  WeeklyStorage connected_minutes_storage_;
  base::WallClockTimer report_timer_;

  base::WallClockTimer connection_report_timer_;
};

}  // namespace brave_vpn

#endif  // BRAVE_COMPONENTS_BRAVE_VPN_BROWSER_BRAVE_VPN_METRICS_H_
