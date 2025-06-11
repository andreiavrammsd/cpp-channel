// Copyright (C) 2020-2025 Andrei Avram

#ifndef MSD_NODISCARD_HPP_
#define MSD_NODISCARD_HPP_

namespace msd {

#if (__cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L))
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif

}  // namespace msd

#endif  // MSD_NODISCARD_HPP_
