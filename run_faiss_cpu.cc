#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>

#include "CLI11.hpp"
#include <faiss/IndexFlat.h>
#include <faiss/IndexHNSW.h>
#include <faiss/IndexIVFFlat.h>
#include <faiss/index_io.h>

#include "utils.h"

/**
 * @brief Create a HNSW index using the CPU
 *
 * @param dim The dimension of the vectors
 * @param ef The number of neighbors to explore
 */
faiss::Index *CPU_create_hnsw_index(int64_t dim, int64_t ef) {
  // Use the default value of M in FAISS
  auto index = new faiss::IndexHNSWFlat(dim, 32);
  // Use the default value of efConstruction in FAISS
  index->hnsw.efConstruction = 40;
  index->hnsw.efSearch = ef;
  return index;
}

int main(int argc, char **argv) {
  CLI::App app{"Run FAISS Benchmarks"};
  argv = app.ensure_utf8(argv);

  std::string dataset;
  app.add_option("-d,--dataset", dataset, "Path to the dataset");

  std::string bf_index_path;
  app.add_option("--bf-index", bf_index_path, "Path to the bruteforce index");

  int64_t learn_limit = 10000;
  app.add_option("--learn-limit", learn_limit,
                 "Limit the number of learn vectors");

  int64_t search_limit = 10000;
  app.add_option("--search-limit", search_limit,
                 "Limit the number of search vectors");

  int64_t top_k = 10;
  app.add_option("-k,--top-k", top_k, "Number of nearest neighbors");

  int64_t ef = 256;
  app.add_option("--ef", ef, "Number of neighbors to explore");

  CLI11_PARSE(app, argc, argv);

  if (dataset.empty()) {
    std::cerr << "[ERROR] Please provide a dataset" << std::endl;
    return 1;
  }

  // Load the learn dataset
  int64_t dim_learn, n_learn;
  float *data_learn;
  std::string dataset_path_learn = dataset + "/dataset.bin";
  read_dataset(dataset_path_learn.c_str(), data_learn, &n_learn, &dim_learn,
               learn_limit);

  // Print information about the learn dataset
  std::cout << "[INFO] Learn dataset shape: " << dim_learn << " x " << n_learn
            << std::endl;
  preview_dataset(data_learn);

  // Set parameters
  int64_t n_list = int64_t(4 * std::sqrt(n_learn));

  // Create the index
  faiss::Index *idx = CPU_create_hnsw_index(dim_learn, ef);

  // Add vectors to the index
  auto s = std::chrono::high_resolution_clock::now();
  idx->add(n_learn, data_learn);
  auto e = std::chrono::high_resolution_clock::now();
  std::cout
      << "[TIME] Index: "
      << std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count()
      << " ms" << std::endl;

  // Load the search dataset
  int64_t dim_query, n_query;
  float *data_query;
  std::string dataset_path_query = dataset + "/query.bin";
  read_dataset(dataset_path_query.c_str(), data_query, &n_query, &dim_query,
               search_limit);

  // Print information about the search dataset
  std::cout << "[INFO] Query dataset shape: " << dim_query << " x " << n_query
            << std::endl;
  preview_dataset(data_query);

  // Containers to hold the search results
  std::vector<faiss::idx_t> nns(top_k * n_query);
  std::vector<float> dis(top_k * n_query);

  // Perform the search
  s = std::chrono::high_resolution_clock::now();
  idx->search(n_query, data_query, top_k, dis.data(), nns.data());
  e = std::chrono::high_resolution_clock::now();
  std::cout
      << "[TIME] Search: "
      << std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count()
      << " ms" << std::endl;

  if (!bf_index_path.empty()) {
    // Run bruteforce experiments
    std::vector<faiss::idx_t> gt_nns(top_k * n_query);
    std::vector<float> gt_dis(top_k * n_query);
    auto brute_force_index = faiss::read_index(bf_index_path.c_str());
    brute_force_index->search(n_query, data_query, top_k, gt_dis.data(),
                              gt_nns.data());

    // Calculate the recall
    int64_t recalls = 0;
    for (int64_t i = 0; i < n_query; ++i) {
      for (int64_t n = 0; n < top_k; n++) {
        for (int64_t m = 0; m < top_k; m++) {
          if (nns[i * top_k + n] == gt_nns[i * top_k + m]) {
            recalls += 1;
          }
        }
      }
    }
    float recall = 1.0f * recalls / (top_k * n_query);
    std::cout << "[INFO] Recall@" << top_k << ": " << recall << std::endl;
  }

  // Delete the datasets
  delete[] data_learn;
  delete[] data_query;

  return 0;
}
