/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated April 5, 2025. Replaces all prior versions.
 *
 * Copyright (c) 2013-2025, Esoteric Software LLC
 *
 * Integration of the Spine Runtimes into software or otherwise creating
 * derivative works of the Spine Runtimes is permitted under the terms and
 * conditions of Section 2 of the Spine Editor License Agreement:
 * http://esotericsoftware.com/spine-editor-license
 *
 * Otherwise, it is permitted to integrate the Spine Runtimes into software
 * or otherwise create derivative works of the Spine Runtimes (collectively,
 * "Products"), provided that each user of the Products must obtain their own
 * Spine Editor license and redistribution of the Products in any form must
 * include this license and copyright notice.
 *
 * THE SPINE RUNTIMES ARE PROVIDED BY ESOTERIC SOFTWARE LLC "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ESOTERIC SOFTWARE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES,
 * BUSINESS INTERRUPTION, OR LOSS OF USE, DATA, OR PROFITS) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THE SPINE RUNTIMES, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#ifndef Spine_PathConstraintSpacingTimeline_h
#define Spine_PathConstraintSpacingTimeline_h

#include <spine/PathConstraintPositionTimeline.h>

namespace spine {
	class SP_API PathConstraintSpacingTimeline : public CurveTimeline1 {
		friend class SkeletonBinary;

		friend class SkeletonJson;

	RTTI_DECL

	public:
		explicit PathConstraintSpacingTimeline(size_t frameCount, size_t bezierCount, int pathConstraintIndex);

		virtual void
		apply(Skeleton &skeleton, float lastTime, float time, Vector<Event *> *pEvents, float alpha, MixBlend blend,
			  MixDirection direction);

		int getPathConstraintIndex() { return _pathConstraintIndex; }

		void setPathConstraintIndex(int inValue) { _pathConstraintIndex = inValue; }

	protected:
		int _pathConstraintIndex;
	};
}

#endif /* Spine_PathConstraintSpacingTimeline_h */
