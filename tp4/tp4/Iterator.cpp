#include "Iterator.hpp"

Iterator::Iterator(const std::vector<std::unique_ptr<Offre>>& offres)
    : offres_(offres), index_(0) {
}

const Offre* Iterator::next() {
    if (index_ < offres_.size()) {
        return offres_[index_++].get();
    }
    return nullptr;
}
