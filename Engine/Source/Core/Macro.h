//
// Created by ilya on 26.02.2026.
//

#ifndef ENGINE_MACRO_H
#define ENGINE_MACRO_H

#ifdef ENGINE_EXPORTS
    #define ENGINE_API __declspec(dllexport)
#else
    #define ENGINE_API __declspec(dllimport)
#endif

#endif //ENGINE_MACRO_H