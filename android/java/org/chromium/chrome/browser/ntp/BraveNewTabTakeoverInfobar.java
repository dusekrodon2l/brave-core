/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

package org.chromium.chrome.browser.ntp;

import org.chromium.base.Log;
import org.chromium.chrome.R;
import org.chromium.chrome.browser.BraveRewardsHelper;
import org.chromium.chrome.browser.app.BraveActivity;
import org.chromium.chrome.browser.flags.ChromeFeatureList;
import org.chromium.chrome.browser.infobar.BraveInfoBarIdentifier;
import org.chromium.chrome.browser.preferences.BravePref;
import org.chromium.chrome.browser.profiles.Profile;
import org.chromium.chrome.browser.tab.Tab;
import org.chromium.chrome.browser.ui.messages.infobar.BraveSimpleConfirmInfoBarBuilder;
import org.chromium.chrome.browser.ui.messages.infobar.SimpleConfirmInfoBarBuilder;
import org.chromium.chrome.browser.util.TabUtils;
import org.chromium.components.prefs.PrefService;
import org.chromium.components.user_prefs.UserPrefs;

public class BraveNewTabTakeoverInfobar {
    private static final String TAG = "NewTabTakeover";
    private static final String LEARN_MORE_URL =
            "https://support.brave.com/hc/en-us/articles/35182999599501";
    private static final String NEW_TAB_PAGE_ADS_FEATURE = "NewTabPageAds";
    private static final String SHOULD_SUPPORT_CONFIRMATIONS_FOR_NON_REWARDS_FEATURE_PARAM =
            "should_support_confirmations_for_non_rewards";
    private final Profile mProfile;

    public BraveNewTabTakeoverInfobar(Profile profile) {
        mProfile = profile;
    }

    public void maybeDisplayAndIncrementCounter() {
        try {
            if (!shouldDisplayInfobar()) {
                return;
            }
            recordInfobarWasDisplayed();

            BraveActivity activity = BraveActivity.getBraveActivity();
            Tab tab = activity.getActivityTabProvider().get();
            if (tab == null) return;

            BraveSimpleConfirmInfoBarBuilder.create(
                    tab.getWebContents(),
                    new SimpleConfirmInfoBarBuilder.Listener() {
                        @Override
                        public void onInfoBarDismissed() {
                            suppressInfobar();
                        }

                        @Override
                        public boolean onInfoBarButtonClicked(boolean isPrimary) {
                            suppressInfobar();
                            return false;
                        }

                        @Override
                        public boolean onInfoBarLinkClicked() {
                            suppressInfobar();
                            TabUtils.openUrlInSameTab(LEARN_MORE_URL);
                            return true;
                        }
                    },
                    BraveInfoBarIdentifier.NEW_TAB_TAKEOVER_INFOBAR_DELEGATE,
                    activity,
                    /* drawableId= */ 0,
                    activity.getString(R.string.new_tab_takeover_infobar_message, ""),
                    /* primaryText= */ "",
                    /* secondaryText= */ "",
                    activity.getString(
                            R.string.new_tab_takeover_infobar_learn_more_opt_out_choices, ""),
                    /* autoExpire= */ true);
        } catch (BraveActivity.BraveActivityNotFoundException e) {
            Log.e(TAG, "maybeCreate infobar", e);
        }
    }

    private boolean shouldDisplayInfobar() {
        if (!ChromeFeatureList.getFieldTrialParamByFeatureAsBoolean(
                NEW_TAB_PAGE_ADS_FEATURE,
                SHOULD_SUPPORT_CONFIRMATIONS_FOR_NON_REWARDS_FEATURE_PARAM,
                /* defaultValue= */ false)) {
            return false;
        }

        if (BraveRewardsHelper.isRewardsEnabled()) {
            return false;
        }

        PrefService prefService = UserPrefs.get(mProfile);
        final int infobarDisplayCount =
                prefService.getInteger(BravePref.NEW_TAB_TAKEOVER_INFOBAR_REMAINING_DISPLAY_COUNT);
        return infobarDisplayCount > 0;
    }

    private void recordInfobarWasDisplayed() {
        PrefService prefService = UserPrefs.get(mProfile);
        final int infobarDisplayCount =
                prefService.getInteger(BravePref.NEW_TAB_TAKEOVER_INFOBAR_REMAINING_DISPLAY_COUNT);
        prefService.setInteger(
                BravePref.NEW_TAB_TAKEOVER_INFOBAR_REMAINING_DISPLAY_COUNT,
                infobarDisplayCount - 1);
    }

    private void suppressInfobar() {
        PrefService prefService = UserPrefs.get(mProfile);
        prefService.setInteger(BravePref.NEW_TAB_TAKEOVER_INFOBAR_REMAINING_DISPLAY_COUNT, 0);
    }
}
