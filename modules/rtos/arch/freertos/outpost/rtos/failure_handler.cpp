/*
 * Copyright (c) 2014-2017, German Aerospace Center (DLR)
 *
 * This file is part of the development version of OUTPOST.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Authors:
 * - 2014-2017, Fabian Greif (DLR RY-AVS)
 */
// ----------------------------------------------------------------------------

#include <outpost/rtos/failure_handler.h>

static void
defaultHandler(outpost::rtos::FailureCode code)
{
    // Avoid warnings about unused parameters.
    (void) code;
}

outpost::rtos::FailureHandler::Handler outpost::rtos::FailureHandler::handler = &defaultHandler;
outpost::rtos::FailureHandler::Handler outpost::rtos::FailureHandler::cleanup = &defaultHandler;
