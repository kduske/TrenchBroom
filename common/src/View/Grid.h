/*
 Copyright (C) 2010-2017 Kristian Duske
 
 This file is part of TrenchBroom.
 
 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TrenchBroom_Grid
#define TrenchBroom_Grid

#include "Macros.h"
#include "TrenchBroom.h"
#include "VecMath.h"
#include "Notifier.h"
#include "Model/ModelTypes.h"

#include <array>

namespace TrenchBroom {
    namespace View {
        class Grid {
        public:
            static const int MaxSize = 8;
            static const int MinSize = -3;
        private:
            int m_size;
            bool m_snap;
            bool m_visible;
        public:
            Notifier0 gridDidChangeNotifier;
        public:
            explicit Grid(const int size);
            
            int size() const;
            void setSize(const int size);
            void incSize();
            void decSize();
            FloatType actualSize() const;
            FloatType angle() const;
        
            bool visible() const;
            void toggleVisible();
            
            bool snap() const;
            void toggleSnap();
            
            template <typename T>
            T snapAngle(const T a) const {
                if (!snap())
                    return a;
                return angle() * Math::round(a / angle());
            }
        public: // Snap scalars.
            template <typename T>
            T snap(const T f) const {
                return snap(f, SnapDir_None);
            }
            
            template <typename T>
            T offset(const T f) const {
                if (!snap())
                    return static_cast<T>(0.0);
                return f - snap(f);
            }
            
            template <typename T>
            T snapUp(const T f, const bool skip) const {
                return snap(f, SnapDir_Up, skip);
            }
            
            template <typename T>
            T snapDown(const T f, const bool skip) const {
                return snap(f, SnapDir_Down, skip);
            }
        private:
            typedef enum {
                SnapDir_None,
                SnapDir_Up,
                SnapDir_Down
            } SnapDir;
            
            template <typename T>
            T snap(const T f, const SnapDir snapDir, const bool skip = false) const {
                if (!snap())
                    return f;

                const T actSize = static_cast<T>(actualSize());
                switch (snapDir) {
                    case SnapDir_None:
                        return Math::snap(f, actSize);
                    case SnapDir_Up: {
                        const T s = actSize * std::ceil(f / actSize);
                        return (skip && Math::eq(s, f)) ? s + static_cast<T>(actualSize()) : s;
                    }
                    case SnapDir_Down: {
                        const T s = actSize * std::floor(f / actSize);
                        return (skip && Math::eq(s, f)) ? s - static_cast<T>(actualSize()) : s;
                    }
					switchDefault()
                }
            }
        public: // Snap vectors.
            template <typename T, size_t S>
            vm::vec<T,S> snap(const vm::vec<T,S>& p) const {
                return snap(p, SnapDir_None);
            }
            
            template <typename T, size_t S>
            vm::vec<T,S> offset(const vm::vec<T,S>& p) const {
                if (!snap())
                    return vm::vec<T,S>::zero;
                return p - snap(p);
            }
            
            template <typename T, size_t S>
            vm::vec<T,S> snapUp(const vm::vec<T,S>& p, const bool skip = false) const {
                return snap(p, SnapDir_Up, skip);
            }
            
            template <typename T, size_t S>
            vm::vec<T,S> snapDown(const vm::vec<T,S>& p, const bool skip = false) const {
                return snap(p, SnapDir_Down, skip);
            }
        private:
            template <typename T, size_t S>
            vm::vec<T,S> snap(const vm::vec<T,S>& p, const SnapDir snapDir, const bool skip = false) const {
                if (!snap())
                    return p;
                vm::vec<T,S> result;
                for (size_t i = 0; i < S; ++i)
                    result[i] = snap(p[i], snapDir, skip);
                return result;
            }
        public: // Snap towards an arbitrary direction.
            template <typename T, size_t S>
            vm::vec<T,S> snapTowards(const vm::vec<T,S>& p, const vm::vec<T,S>& d, const bool skip = false) const {
                if (!snap())
                    return p;
                vm::vec3 result;
                for (size_t i = 0; i < S; ++i) {
                    if (    Math::pos(d[i]))    result[i] = snapUp(p[i], skip);
                    else if(Math::neg(d[i]))    result[i] = snapDown(p[i], skip);
                    else                        result[i] = snap(p[i]);
                }
                return result;
            }
        public: // Snapping on a plane! Surprise, motherfucker!
            template <typename T>
            vm::vec<T,3> snap(const vm::vec<T,3>& p, const vm::plane<T,3>& onPlane) const {
                return snap(p, onPlane, SnapDir_None, false);
            }
            
            template <typename T>
            vm::vec<T,3> snapUp(const vm::vec<T,3>& p, const vm::plane<T,3>& onPlane, const bool skip = false) const {
                return snap(p, onPlane, SnapDir_Up, skip);
            }
            
            template <typename T>
            vm::vec<T,3> snapDown(const vm::vec<T,3>& p, const vm::plane<T,3>& onPlane, const bool skip = false) const {
                return snap(p, onPlane, SnapDir_Down, skip);
            }

            template <typename T, size_t S>
            vm::vec<T,S> snapTowards(const vm::vec<T,S>& p, const vm::plane<T,3>& onPlane, const vm::vec<T,S>& d, const bool skip = false) const {
                
                SnapDir snapDirs[S];
                for (size_t i = 0; i < S; ++i)
                    snapDirs[i] = (d[i] < 0.0 ? SnapDir_Down : (d[i] > 0.0 ? SnapDir_Up : SnapDir_None));

                return snap(p, onPlane, snapDirs, skip);
            }
        private:
            template <typename T, size_t S>
            vm::vec<T,3> snap(const vm::vec<T,S>& p, const vm::plane<T,S>& onPlane, const SnapDir snapDir, const bool skip = false) const {
                SnapDir snapDirs[S];
                for (size_t i = 0; i < S; ++i)
                    snapDirs[i] = snapDir;
                
                return snap(p, onPlane, snapDirs, skip);
            }
            
            template <typename T, size_t S>
            vm::vec<T,S> snap(const vm::vec<T,S>& p, const vm::plane<T,3>& onPlane, const SnapDir snapDirs[], const bool skip = false) const {
                
                vm::vec<T,3> result;
                switch(firstComponent(onPlane.normal)) {
                    case Math::Axis::AX:
                        result[1] = snap(p.y(), snapDirs[1], skip);
                        result[2] = snap(p.z(), snapDirs[2], skip);
                        result[0] = onPlane.xAt(result.yz());
                        break;
                    case Math::Axis::AY:
                        result[0] = snap(p.x(), snapDirs[0], skip);
                        result[2] = snap(p.z(), snapDirs[2], skip);
                        result[1] = onPlane.yAt(result.xz());
                        break;
                    case Math::Axis::AZ:
                        result[0] = snap(p.x(), snapDirs[0], skip);
                        result[1] = snap(p.y(), snapDirs[1], skip);
                        result[2] = onPlane.zAt(result.xy());
                        break;
                }
                return result;
            }
        public:
            // Snapping on an a line means finding the closest point on a line such that at least one coordinate
            // is on the grid, ignoring a coordinate if the line direction is identical to the corresponding axis.
            template <typename T>
            vm::vec<T,3> snap(const vm::vec<T,3>& p, const vm::line<T,3> line) const {
                // Project the point onto the line.
                const vm::vec<T,3> pr = line.projectPoint(p);
                const T prDist = line.distanceToProjectedPoint(pr);
                
                vm::vec<T,3> result = pr;
                T bestDiff = std::numeric_limits<T>::max();
                for (size_t i = 0; i < 3; ++i) {
                    if (line.direction[i] != 0.0) {
                        const std::array<T,2> v = { {snapDown(pr[i], false) - line.point[i], snapUp(pr[i], false) - line.point[i]} };
                        for (size_t j = 0; j < 2; ++j) {
                            const T s = v[j] / line.direction[i];
                            const T diff = Math::absDifference(s, prDist);
                            if (diff < bestDiff) {
                                result = line.pointAtDistance(s);
                                bestDiff = diff;
                            }
                        }
                    }
                }
                
                return result;
            }
            
            template <typename T>
            vm::vec<T,3> snap(const vm::vec<T,3>& p, const vm::segment<T,3> edge) const {
                const vm::vec<T,3> v = edge.end() - edge.start();
                const T len = length(v);
                
                const vm::vec<T,3> orig = edge.start();
                const vm::vec<T,3> dir = v / len;
                
                const vm::vec<T,3> snapped = snap(p, vm::line<T,3>(orig, dir));
                const T dist = dot(dir, snapped - orig);
                if (dist < 0.0 || dist > len)
                    return vm::vec<T,3>::NaN;
                
                return snapped;
            }
            
            template <typename T>
            vm::vec<T,3> snap(const vm::vec<T,3>& p, const vm::polygon<T,3>& polygon, const vm::vec<T,3>& normal) const {
                ensure(polygon.vertexCount() >= 3, "polygon has too few vertices");
                
                const vm::plane<T,3> plane(polygon.vertices().front(), normal);
                vm::vec<T,3> ps = snap(p, plane);
                T err = squaredLength(p - ps);
                
                if (!polygon.contains(ps, plane.normal)) {
                    ps = vm::vec<T,3>::NaN;
                    err = std::numeric_limits<T>::max();
                }
                
                auto last = std::begin(polygon);
                auto cur = std::next(last);
                auto end = std::end(polygon);
                
                while (cur != end) {
                    const vm::vec<T,3> cand = snap(p, vm::segment<T,3>(*last, *cur));
                    if (!isNaN(cand)) {
                        const T cerr = squaredLength(p - cand);
                        if (cerr < err) {
                            err = cerr;
                            ps = cand;
                        }
                    }
                    
                    last = cur;
                    ++cur;
                }
                
                return ps;
            }
        public:
            FloatType intersectWithRay(const ray3& ray, const size_t skip) const;
            
            /**
             * Returns a copy of `delta` that snaps the result to grid, if the grid snapping moves the result in the same direction as delta (tested on each axis).
             * Otherwise, returns the original point for that axis.
             */
            vm::vec3 moveDeltaForPoint(const vm::vec3& point, const vm::bbox3& worldBounds, const vm::vec3& delta) const;
            /**
             * Returns a delta to `bounds.mins` which moves the box to point where `ray` impacts `dragPlane`, grid snapped.
             * The box is positioned so it is in front of `dragPlane`.
             */
            vm::vec3 moveDeltaForBounds(const vm::plane3& dragPlane, const vm::bbox3& bounds, const vm::bbox3& worldBounds, const ray3& ray, const vm::vec3& position) const;
            vm::vec3 moveDelta(const vm::bbox3& bounds, const vm::bbox3& worldBounds, const vm::vec3& delta) const;
            vm::vec3 moveDelta(const vm::vec3& point, const vm::bbox3& worldBounds, const vm::vec3& delta) const;
            vm::vec3 moveDelta(const vm::vec3& delta) const;
            /**
             * Given `delta`, a vector in the direction of the face's normal,
             * returns a copy of it, also in the direction of the face's normal, that will try to keep the face on-grid.
             */
            vm::vec3 moveDelta(const Model::BrushFace* face, const vm::vec3& delta) const;
            vm::vec3 combineDeltas(const vm::vec3& delta1, const vm::vec3& delta2) const;
            vm::vec3 referencePoint(const vm::bbox3& bounds) const;
        };
    }
}

#endif /* defined(TrenchBroom_Grid) */
