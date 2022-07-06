/* -*- c++ -*- */
#pragma once

#ifdef CLIENT_DLL
#define DLLEXP __declspec(dllexport)
#else
#define DLLEXP __declspec(dllimport)
#endif
