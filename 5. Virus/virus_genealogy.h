#ifndef VIRUS_VIRUS_GENEALOGY_H
#define VIRUS_VIRUS_GENEALOGY_H

#include <vector>

template<typename Virus>
class VirusGenealogy {
private:


public:

    constexpr VirusGenealogy(typename Virus::id_type const &stem_id);

    constexpr typename Virus::id_type get_stem_id() const;

    // trzeba samemu zdefiniować children_iterator
    constexpr VirusGenealogy<Virus>::children_iterator get_children_begin(typename Virus::id_type const &id) const;

    // trzeba samemu zdefiniować children_iterator
    constexpr VirusGenealogy<Virus>::children_iterator get_children_end(typename Virus::id_type const &id) const;

    std::vector<typename Virus::id_type> get_parents(typename Virus::id_type const &id) const;

    bool exists(typename Virus::id_type const &id) const;

    const Virus& operator[](typename Virus::id_type const &id) const;


    void create(typename Virus::id_type const &id, typename Virus::id_type const &parent_id);
    void create(typename Virus::id_type const &id, std::vector<typename Virus::id_type> const &parent_ids);

    void connect(typename Virus::id_type const &child_id, typename Virus::id_type const &parent_id);

    void remove(typename Virus::id_type const &id);
};

#endif //VIRUS_VIRUS_GENEALOGY_H
