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

#ifndef TRENCHBROOM_SEGMENT_DECL_H
#define TRENCHBROOM_SEGMENT_DECL_H

#include "abstract_line.h"
#include "vec_decl.h"

#include <vector>

/**
 * A line segment, represented by its two end points.
 *
 * This class enforces the following invariant: the start point of the segment is always less than or equal to
 * the end point.
 *
 * @tparam T the component type
 * @tparam S the number of components
 */
// TODO 2201: introduce abstract_line concept with origin, direction, positiveLength, negativeLength
template <typename T, size_t S>
class segment : public abstract_line<T,S> {
public:
    using Type = T;
    static const size_t Size = S;
    using float_type = segment<float, S>;
    using List = std::vector<segment<T,S>>;
private:
    vec<T,S> m_start;
    vec<T,S> m_end;
public:
    /**
     * Creates a new empty segment of length 0 with both the start and the end set to 0.
     */
    segment();

    /**
     * Creates a new segment with the given points.
     *
     * @param p1 one end point
     * @param p2 the opposite end point
     */
    segment(const vec<T,S>& p1, const vec<T,S>& p2);

    // Copy and move constructors
    segment(const segment<T,S>& other) = default;
    segment(segment<T,S>&& other) noexcept = default;

    // Assignment operators
    segment<T,S>& operator=(const segment<T,S>& other) = default;
    segment<T,S>& operator=(segment<T,S>&& other) noexcept = default;

    /**
     * Creates a new segment by copying the values from the given segment. If the given segment has a different
     * component type, the values are converted using static_cast.
     *
     * @tparam U the component type of the given segment
     * @param other the segment to copy the values from
     */
    template <typename U>
    explicit segment(const segment<U,S>& other) :
    m_start(other.start()),
    m_end(other.end()) {}

    // implement abstract_line interface
    vec<T,S> getOrigin() const override;
    vec<T,S> getDirection() const override;

    /**
     * Transforms this segment using the given transformation matrix.
     *
     * @param transform the transformation to apply
     * @return the transformed segment
     */
    segment<T,S> transform(const mat<T,S+1,S+1>& transform) const;

    /**
     * Returns the start point of this segment.
     *
     * @return the start point
     */
    const vec<T,S>& start() const;

    /**
     * Returns the end point of this segment.
     *
     * @return the end point
     */
    const vec<T,S>& end() const;

    /**
     * Returns the center point of this segment.
     *
     * @return the center point
     */
    vec<T,S> center() const;

    /**
     * Returns the normalized direction vector of this segment, i.e., a unit vector which points at the end
     * point, assuming the start point is the origin of the vector.
     *
     * @return the direction vector
     */
    vec<T,S> direction() const;

    /**
     * Adds the start and end points of the given range of segments to the given output iterator.
     *
     * @tparam I the range iterator type
     * @tparam O the output iterator type
     * @param cur the range start
     * @param end the range end
     * @param out the output iterator
     */
    template <typename I, typename O>
    static void getVertices(I cur, I end, O out) {
        while (cur != end) {
            const auto& segment = *cur;
            out = segment.start(); ++out;
            out = segment.end(); ++out;
            ++cur;
        }
    }
private:
    void flip() {
        using std::swap;
        swap(m_start, m_end);
    }
};

/**
 * Compares the given segments using the given epsilon value. Thereby, the start points of the segments are
 * compared first, and if the comparison yields a value other than 0, that value is returned. Otherwise, the
 * result of comparing the end points is returned.
 *
 * Note that by the invariant of the segment class, the start point is always less than or equal to the end
 * point.
 *
 * @tparam T the component type
 * @tparam S the number of components
 * @param lhs the first segment
 * @param rhs the second segment
 * @param epsilon an epsilon value
 * @return -1 if the first segment is less than the second segment, +1 in the opposite case, and 0 if the
 * segments are equal
 */
template <typename T, size_t S>
int compare(const segment<T,S>& lhs, const segment<T,S>& rhs, T epsilon = static_cast<T>(0.0));

/**
 * Checks if the first given segment identical to the second segment.
 *
 * @tparam T the component type
 * @tparam S the number of components
 * @param lhs the first segment
 * @param rhs the second segment
 * @return true if the segments are identical and false otherwise
 */
template <typename T, size_t S>
bool operator==(const segment<T,S>& lhs, const segment<T,S>& rhs);

/**
 * Checks if the first given segment identical to the second segment.
 *
 * @tparam T the component type
 * @tparam S the number of components
 * @param lhs the first segment
 * @param rhs the second segment
 * @return false if the segments are identical and true otherwise
 */
template <typename T, size_t S>
bool operator!=(const segment<T,S>& lhs, const segment<T,S>& rhs);

/**
 * Checks if the first given segment less than the second segment.
 *
 * @tparam T the component type
 * @tparam S the number of components
 * @param lhs the first segment
 * @param rhs the second segment
 * @return true if the first segment is less than the second and false otherwise
 */
template <typename T, size_t S>
bool operator<(const segment<T,S>& lhs, const segment<T,S>& rhs);

/**
 * Checks if the first given segment less than or equal to the second segment.
 *
 * @tparam T the component type
 * @tparam S the number of components
 * @param lhs the first segment
 * @param rhs the second segment
 * @return true if the first segment is less than or equal to the second and false otherwise
 */
template <typename T, size_t S>
bool operator<=(const segment<T,S>& lhs, const segment<T,S>& rhs);

/**
 * Checks if the first given segment greater than the second segment.
 *
 * @tparam T the component type
 * @tparam S the number of components
 * @param lhs the first segment
 * @param rhs the second segment
 * @return true if the first segment is greater than the second and false otherwise
 */
template <typename T, size_t S>
bool operator>(const segment<T,S>& lhs, const segment<T,S>& rhs);

/**
 * Checks if the first given segment greater than or equal to the second segment.
 *
 * @tparam T the component type
 * @tparam S the number of components
 * @param lhs the first segment
 * @param rhs the second segment
 * @return true if the first segment is greater than or equal to the second and false otherwise
 */
template <typename T, size_t S>
bool operator>=(const segment<T,S>& lhs, const segment<T,S>& rhs);

/**
 * Translates the given segment by the given offset.
 *
 * @tparam T the component type
 * @tparam S the number of components
 * @param s the segment to translate
 * @param offset the offset
 * @return the translated segment
 */
template <typename T, size_t S>
segment<T,S> translate(const segment<T,S>& s, const vec<T,S>& offset);

#endif
