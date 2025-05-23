/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_ads/core/internal/serving/prediction/model_based/weight/creative_inline_content_ad_model_based_predictor_weights_builder.h"

#include "brave/components/brave_ads/core/internal/common/test/test_base.h"
#include "brave/components/brave_ads/core/internal/serving/prediction/model_based/weight/creative_ad_model_based_predictor_weights_info.h"

// npm run test -- brave_unit_tests --filter=BraveAds*

namespace brave_ads {

class BraveAdsCreativeInlineContentAdModelBasedPredictorWeightsBuilderTest
    : public test::TestBase {};

TEST_F(BraveAdsCreativeInlineContentAdModelBasedPredictorWeightsBuilderTest,
       BuildCreativeInlineContentAdModelBasedPredictorWeights) {
  // Act
  const CreativeAdModelBasedPredictorWeightsInfo weights =
      BuildCreativeInlineContentAdModelBasedPredictorWeights();

  // Assert
  CreativeAdModelBasedPredictorWeightsInfo expected_weights;
  expected_weights.intent_segment.child = 0.0;
  expected_weights.intent_segment.parent = 0.0;
  expected_weights.latent_interest_segment.child = 0.0;
  expected_weights.latent_interest_segment.parent = 0.0;
  expected_weights.interest_segment.child = 0.0;
  expected_weights.interest_segment.parent = 0.0;
  expected_weights.untargeted_segment = 0.0001;
  expected_weights.last_seen_ad = 0.0;
  EXPECT_EQ(expected_weights, weights);
}

}  // namespace brave_ads
