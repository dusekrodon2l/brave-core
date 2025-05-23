/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import * as React from 'react'
import { createRoot } from 'react-dom/client'
import { setIconBasePath } from '@brave/leo/react/icon'

import { AppModelContext } from './lib/app_model_context'
import { createModel } from './webui/webui_model'
import { App } from './components/app'

setIconBasePath('chrome://resources/brave-icons')

const model = createModel()
const root = createRoot(document.getElementById('root')!)

root.render(
  <AppModelContext.Provider value={model}>
    <App />
  </AppModelContext.Provider>
)
