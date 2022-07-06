/* -*- c++ -*- */

#pragma once

#define _HL2_BASIC_GETTER(T, I) inline T Get ## I ## ()  { return m_ ## I ## ; }
#define _HL2_BASIC_EQCMP(I, A, E) inline bool Is ## I ## () { return (A) == (E); }
