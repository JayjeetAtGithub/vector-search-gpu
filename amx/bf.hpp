#pragma once

#include <queue>
#include <vector>
#include <iostream>
#include <map>

#include "distance.hpp"

struct Comp {
  static bool operator()(const std::pair<int, float> &a, const std::pair<int, float> &b) {
    return a.second < b.second;
  }
};

class BruteForceSearch {
  int32_t _dim;

  dnnl::engine engine;
  dnnl::stream stream;

public:
  void init_onednn() {
    engine = dnnl::engine(dnnl::engine::kind::cpu, 0);
    stream = dnnl::stream(engine);
  }

  BruteForceSearch(int32_t dim) : _dim(dim) {
    init_onednn();
    if (!is_amxbf16_supported()) {
      std::cout << "Intel AMX unavailable" << std::endl;
    }
  }

  void search_ip_amx(
    std::vector<bf16> &queries, int32_t nq,
    std::vector<bf16> &dataset, int32_t nl, int32_t top_k) {

    std::vector<bf16> distances;
    distances.resize((int64_t)nq * (int64_t)nl);

    std::unordered_map<
      int32_t, 
      std::priority_queue<
        std::pair<int, float>, 
        std::vector<std::pair<int, float>>, 
        Comp
      >>
    m;

    amx_inner_product(
      nq, nl, _dim, queries, dataset, distances, engine, stream
    );

    // preview distances
    for (int32_t i = 0; i < 5; i++) {
      for (int32_t j = 0; j < 10; j++) {
        std::cout << distances[i * nl + j] << " ";
      }
      std::cout << std::endl;
    }

    // for (int32_t i = 0; i < nq; i++) {
    //   for (int32_t j = 0; j < nl; j++) {
    //     m[i].push({j, distances[i * nl + j]});
    //   }
    // }

    // std::vector<std::vector<int>> results(
    //   nq, std::vector<int>(top_k)
    // );

    // for (int i = 0; i < nq; i++) {
    //   int32_t k_idx = 0;
    //   while (k_idx < top_k) {
    //     results[i][k_idx++] = m[i].top().first;
    //     m[i].pop();
    //   }
    // }

  }
};
