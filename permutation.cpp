#include <iostream>

bool i_not_in_current(int *&current_indexes, int &cur_array, int &i) {
  bool i_is_not_in_current = true;
  for (int j = 0; j < cur_array; ++j) {
    if (i == current_indexes[j]) {
      i_is_not_in_current = false;
    }
  }
  return i_is_not_in_current;
}

void indexes_without_forbidden(int cur_array, int *&current_indexes,
                               int *&sizes, int &argc, long long &answer,
                               int **&arrays) {
  if (cur_array == argc - 1 && argc != 1) {
    long long product = 1;
    for (int i = 0; i < argc - 1; ++i) {
      product *= arrays[i][current_indexes[i]];
    }
    answer += product;
    return;
  }
  int size = sizes[cur_array];
  for (int i = 0; i < size; ++i) {
    if (i_not_in_current(current_indexes, cur_array, i)) {
      current_indexes[cur_array] = i;
      indexes_without_forbidden(cur_array + 1, current_indexes, sizes, argc,
                                answer, arrays);
    }
  }
}

int main(int argc, const char *argv[]) {
  int **arrays = new int *[argc - 1];
  int *sizes = new int[argc - 1];
  for (int i = 1; i < argc; ++i) {
    sizes[i - 1] = std::atoi(argv[i]);
  }
  long long sum = 0;
  for (int i = 1; i < argc; ++i) {
    arrays[i - 1] = new int[std::atoi(argv[i])];
    int size = std::atoi(argv[i]);
    for (int j = 0; j < size; ++j) {
      std::cin >> arrays[i - 1][j];
    }
  }
  int *current_indexes = new int[argc - 1];
  indexes_without_forbidden(0, current_indexes, sizes, argc, sum, arrays);
  std::cout << sum;
  for (int i = 0; i < argc - 1; ++i) {
    delete[] arrays[i];
  }
  delete[] arrays;
  delete[] sizes;
  delete[] current_indexes;
  return 0;
}
