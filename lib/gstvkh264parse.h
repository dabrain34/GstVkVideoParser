/* VideoParser
 * Copyright (C) 2022 Igalia, S.L.
 *     Author: Víctor Jáquez <vjaquez@igalia.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you
 * may not use this file except in compliance with the License.  You
 * may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.  See the License for the specific language governing
 * permissions and limitations under the License.
 */

#pragma once

#include <gstvkparse.h>
#include <gst/codecparsers/gsth264parser.h>
#include "vkvideodecodeparser.h"

G_BEGIN_DECLS

typedef struct _VkH264Picture VkH264Picture;
struct _VkH264Picture {
    StdVideoH264HrdParameters hrd;
    StdVideoH264SequenceParameterSetVui vui;
    StdVideoH264SequenceParameterSet sps;
    StdVideoH264PictureParameterSet pps;
    StdVideoH264ScalingLists scaling_lists_sps, scaling_lists_pps;
    int32_t offset_for_ref_frame[255];
};

class GstVkH264Parser : public GstVkParser {
public:
    GstVkH264Parser(VkParserVideoDecodeClient* client);
    ~GstVkH264Parser();

    GstFlowReturn parse(GstBuffer* buffer);
    GstFlowReturn identifyNalu(GstH264NalUnit* nalu);

    GstFlowReturn parseSPS(GstH264NalUnit * nalu);
    GstFlowReturn parsePPS(GstH264NalUnit * nalu);
    GstFlowReturn parseSlice(GstH264NalUnit * nalu);

private:
    GstH264NalParser* m_parser;
    VkParserVideoDecodeClient* m_client;

    VkH264Picture vkp;
    
    VkSharedBaseObj<VkParserVideoRefCountBase> spsclient, ppsclient;

    guint32 sps_update_count;
    guint32 pps_update_count;
};

G_END_DECLS
