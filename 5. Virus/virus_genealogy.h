#ifndef VIRUS_VIRUS_GENEALOGY_H
#define VIRUS_VIRUS_GENEALOGY_H

#include <vector>
#include <memory>
#include <map>
#include <set>
#include <iterator>
#include <iostream>

class VirusNotFoundException : public std::exception {
    virtual const char* what() const throw() {
        return "VirusNotFound";
    }
};

class VirusAlreadyCreatedException : public std::exception {
    virtual const char* what() const throw() {
        return "VirusAlreadyCreated";
    }
};

class TriedToRemoveStemVirusException : public std::exception {
    virtual const char* what() const throw() {
        return "TriedToRemoveStemVirus";
    }
};

template<class Virus>
class VirusGenealogy {
public:
    class Node {
    public:
        Virus virus;
        std::map<typename Virus::id_type, std::shared_ptr<Node>> children;
        std::set<typename Virus::id_type> parents;

        Node(const Virus::id_type &virus_id) : virus(virus_id) {};
        Node() {};
        ~Node() {/*
          std::cout << "START\n";
          for (auto child = children.begin(); child != children.end(); ++child) {
              children.erase(child);
          }
          std::cout << "END\n"; */
        };

        std::vector<typename Virus::id_type> parents_vector() const {
            return std::vector(parents.begin(), parents.end());
        }

        void add_parent(const Virus::id_type &parent_id) {
            parents.insert(parent_id);
        };

        void add_child(std::shared_ptr<Node>& child_virus) {
            std::shared_ptr<Node> pointer_to_child(child_virus);
            children.insert({child_virus->virus.get_id(), pointer_to_child});
        };

        Node* create_and_add_child(typename Virus::id_type const &child_id) {
            std::shared_ptr<Node> pointer_to_child = make_shared<Node>(child_id);
            children.insert({child_id, pointer_to_child});
            return pointer_to_child.get();
        };
    };

    std::map<typename Virus::id_type, std::shared_ptr<Node>> viral_map;
    std::shared_ptr<Node> stem_node;

public:
    class children_iterator {
    public:
      std::map<typename Virus::id_type, std::shared_ptr<Node>>::iterator iter;
      children_iterator() : iter() { };
      children_iterator(std::map<typename Virus::id_type, std::shared_ptr<Node>>::iterator const &iter) : iter(iter) { };
      using difference_type = std::ptrdiff_t;
      using value_type = const Virus;
      using pointer = const Virus*;
      using reference = const Virus&;
      using iterator_category = std::bidirectional_iterator_tag;

      const Virus& operator*() const {
          return iter->second->virus;
      };

      children_iterator& operator++() {
          ++iter;
          return *this;
      };

      children_iterator operator++(int) {
          auto copy = iter;
          ++iter;
          return copy;
      };

      children_iterator& operator--() {
          --iter;
          return *this;
      };

      children_iterator operator--(int) {
          auto copy = iter;
          --iter;
          return copy;
      };

      bool operator==(const children_iterator &ch_iter) const {
          return this->iter == ch_iter.iter;
      };

      bool operator!=(const children_iterator &ch_iter) const {
          return this->iter != ch_iter.iter;
      };

      const Virus* operator->() const {
          return &(iter->second->virus);
      };
  };


    //Wydaje mi się, że to nie może być constexpr (chyba sama mapa nie może, więc to tym bardziej)
    VirusGenealogy(Virus::id_type const &stem_id) {
        stem_node = std::make_shared<Node>(stem_id);
        viral_map.insert({stem_id, std::shared_ptr(stem_node)});
    };

    //Jeśli konstruktor nie może, to chyba wgl nie ma sensu robić rzeczy constexpr
    Virus::id_type get_stem_id() const {
        return stem_node.get()->virus.get_id();
    };

    // TODO
    // trzeba samemu zdefiniować children_iterator
    VirusGenealogy<Virus>::children_iterator get_children_begin(typename Virus::id_type const &id) const {
        return children_iterator(viral_map.at(id)->children.begin());
    };

    // TODO
    // trzeba samemu zdefiniować children_iterator
    VirusGenealogy<Virus>::children_iterator get_children_end(typename Virus::id_type const &id) const {
        return children_iterator(viral_map.at(id)->children.end());
    };

    // TODO
    std::vector<typename Virus::id_type> get_parents(typename Virus::id_type const &id) const {
        if (!exists(id)) {
            throw VirusNotFoundException();
        }
        return viral_map.at(id)->parents_vector(); // tutaj można tak czy trzeba przez copy() ?
        //To chyba i tak kopiuje zawartość. Trochę nie jestem jednak pewien czy nie może rzucać wyjątków w trakcie (ale to bym zostawił na potem)
    };

    bool exists(typename Virus::id_type const &id) const {
        return viral_map.contains(id);
        //Czy może tutaj zostać rzucony wyjątek? (np jak operator porównania rzuci) <- to pytanie dotyczy wsm każdej funkcji xd
    };

    const Virus& operator[](typename Virus::id_type const &id) const {
        if (!exists(id)) {
            throw VirusNotFoundException();
        }
        return viral_map.at(id)->virus;
    };

    void create(typename Virus::id_type const &id, typename Virus::id_type const &parent_id) {
        if (exists(id)) {
            throw VirusAlreadyCreatedException();
        }
        if (!exists(parent_id)) {
            throw VirusNotFoundException();
        }

        std::shared_ptr<Node> new_virus = make_shared<Node>(id);
        viral_map[id] = new_virus;
        viral_map[parent_id]->add_child(new_virus);
        new_virus->add_parent(parent_id);
    };

    void create(typename Virus::id_type const &id, std::vector<typename Virus::id_type> const &parent_ids) {
        if (exists(id)) {
            throw VirusAlreadyCreatedException();
        }
        for (auto parent : parent_ids) {
            if (!exists(parent)) {
                throw VirusNotFoundException();
            }
        }

        std::shared_ptr<Node> new_virus = make_shared<Node>(id);
        viral_map[id] = new_virus;

        for (auto i = 0; i < parent_ids.size(); ++i) {
            viral_map[parent_ids[i]]->add_child(new_virus);
            new_virus->add_parent(parent_ids[i]);
        } 
    };

    void connect(typename Virus::id_type const &child_id, typename Virus::id_type const &parent_id) {
        if (!exists(child_id) || !exists(parent_id)) {
            throw VirusNotFoundException();
        }

        viral_map[child_id]->add_parent(parent_id);
        viral_map[parent_id]->add_child(viral_map[child_id]);
    };

    // TODO
    void remove(Virus::id_type const &id) {
        if (!exists(id)) {
            throw VirusNotFoundException();
        }
        if (id == get_stem_id()) {
            throw TriedToRemoveStemVirusException();
        }
        /*
        std::cout << "remove " << id << std::endl << "CZYTANIE MAPY\n";
        for (auto x = viral_map.begin(); x != viral_map.end(); ++x) {
          std::cout << "TO JEST NODE " << x->first << std::endl << "RODZICOW: " << x->second->parents.size() << "\nDZIECI: \n";
          for (auto child = x->second->children.begin(); child != x->second->children.end(); ++child) {
            std::cout << "TO JEST DZIECKO: " << child->first << " z shared_ptr = " << child->second.use_count() << std::endl;

          }
        } */

        try {
            std::vector<typename std::map<typename Virus::id_type, std::shared_ptr<Node>>::iterator> to_be_erased;
            //Tę mapę można chyba zamienić na wektor (set jest posortowany), ale utrzymałem w jednej konwencji
            std::multimap<std::shared_ptr<Node>, typename std::set<typename Virus::id_type> :: iterator> erased_children;
            std::map<std::shared_ptr<Node>, typename std::map<typename Virus::id_type, std::shared_ptr<Node>>::iterator> erased_parents;
            add_erased(to_be_erased, erased_parents, erased_children, id);

            for (auto child : erased_children) {
                std::cout << child.first->virus.get_id() << std::endl;
                child.first->parents.erase(child.second);
            }

            for (auto parent : erased_parents) {
                parent.first->children.erase(parent.second);
            }

            //Jeśli tutaj dotarliśmy bez wyjątku to reszta się wykona -> nic tam nie jest rzucane
            for (auto erase_from_map : to_be_erased) {
                viral_map.erase(erase_from_map);
            }
            //Po usunięciu ostatniego shared pointera do usuwanego wierzchołka wymazane zostanie całe poddrzewo
        }
        catch (int e) {
            //Coś rzucić
        }
    };

private: //Tutaj wrzucę jakieś pomocnicze funkcje, do przeniesienia potem

    //Rekurencyjnie dodaje do to_be_erased iteratory do wierzchołków
    // to_be_erased -> iteratory do usunięcia z viral_map
    // erased_children -> mapa, dla każdego ojca (klucz) zapisuje iteratory do miejsc w secie dzieci do usunięcia
    // erased_paretns -> iteratory do wierzchołka u ojców usuwanego
    void add_erased(std::vector<typename std::map<typename Virus::id_type, std::shared_ptr<Node>>::iterator>& to_be_erased,
                    std::map<std::shared_ptr<Node>, typename std::map<typename Virus::id_type, std::shared_ptr<Node>>::iterator>& erased_parents,
                    std::multimap<std::shared_ptr<Node>, typename std::set<typename Virus::id_type>::iterator>& erased_children,
                    Virus::id_type const &id,
                    bool first = true) {
        auto to_erase = viral_map.find(id);
        to_be_erased.push_back(to_erase);

        for (auto child = to_erase->second->children.begin(); child != to_erase->second->children.end(); ++child) {
            //^iterator do pierwszego dziecka
            auto child_pointer = child->second;
            auto parent_at_child = child_pointer->parents.find(id);
            //^iterator do rodzica
            erased_children.insert({child_pointer, parent_at_child});
            if (erased_children.count(child_pointer) == child_pointer->parents.size()) {
                add_erased(to_be_erased, erased_parents, erased_children, child_pointer->virus.get_id(), false);
            }
        }
        
        if (first)        //Żaden ojciec inny niż ojciec pierwszego usuwanego wierzchołka nie straci dziecka
            for (auto parent = to_erase->second->parents.begin(); parent != to_erase->second->parents.end(); ++parent) {
                auto parent_pointer = viral_map.find(*parent)->second;
                erased_parents.insert({parent_pointer, parent_pointer->children.find(id)});
            }
    };
};

#endif //VIRUS_VIRUS_GENEALOGY_H
