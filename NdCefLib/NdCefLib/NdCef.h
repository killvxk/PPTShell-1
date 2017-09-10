#ifndef _NDCEF_H
#define _NDCEF_H

#ifdef NDCEF_EXPORTS
#define NDCEF_API __declspec(dllexport)
#else
#define NDCEF_API __declspec(dllimport)
#endif

#endif