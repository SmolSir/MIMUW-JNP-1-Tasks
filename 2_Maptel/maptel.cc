#include "maptel.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <unordered_set>
#include <cstring>
#include <cassert>
#include <cctype>

#ifndef NDEBUG
static const bool debug = true;
#else
static const bool debug = false;
#endif

using namespace std;

using dict_map = unordered_map<string, string>;
using global_map = unordered_map<unsigned long, dict_map>;

static unsigned long number_of_dict = 0;

static global_map& maps() {
    static auto* ans = new global_map();

    if (debug && ans == NULL) {
        cerr << "maptel: failed to initialize register of maps\n";
        assert(false);
    }

    return *ans;
}

static inline bool correct_tel_chars(string tel) {
    for (auto c : tel) {
        if (!isdigit(c) && c != 0) {
            return false;
        }
    }
    return true;
}


static inline void maptel_create_debug(unsigned long id) {
    if (maps().find(id) == maps().end()) {
        cerr << "maptel: maptel_create: failed to create map " << id << "\n";
        assert(false);
    }
    cerr << "maptel: maptel_create: new map id = " << number_of_dict << "\n";
}


unsigned long jnp1::maptel_create(void) {
    if (debug) {
        cerr << "maptel: maptel_create()\n";
    }

    maps()[number_of_dict];

    if (debug) {
        maptel_create_debug(number_of_dict);
    }

    unsigned long temp = number_of_dict;
    number_of_dict++;
    return temp;
}


static inline void maptel_predelete_debug(unsigned long id) {
    if (maps().find(id) == maps().end()) {
        cerr << "maptel: maptel_delete: map " << id << " doesn't exists\n";
        assert(false);
    }
}


static inline void maptel_postdelete_debug(unsigned long id) {
    if (maps().find(id) == maps().end()) {
        cerr << "maptel: maptel_delete: map " << id << " deleted\n";
    }
    else {
        cerr << "maptel: maptel_delete: failed to delete map " << id << "\n";
        assert(false);
    }
}


void jnp1::maptel_delete(unsigned long id) {

    if (debug) {
        cerr << "maptel: maptel_delete(" << id << ")\n";
        maptel_predelete_debug(id);
    }

    maps().erase(id);

    if (debug) {
        maptel_postdelete_debug(id);
    }
}


static inline void maptel_preinsert_debug(unsigned long id, char const *tel_src, char const *tel_dst) {

    if (maps().find(id) == maps().end()) {
        cerr << "maptel: maptel_insert: map doesn't exist\n";
        assert(false);
    }


    if (tel_src == NULL || tel_dst == NULL) {
        cerr << "maptel: maptel_insert: pointer is null\n";
        assert(false);
    }

    if (strcmp(tel_src, "") == 0) {
        cerr << "maptel: maptel_insert: tel_src is empty\n";
        assert(false);
    }

    if (strcmp(tel_dst, "") == 0) {
        cerr << "maptel: maptel_insert: tel_dst is empty\n";
        assert(false);
    }

    cerr << "maptel: maptel_insert(" << id << ", " << tel_src << ", " << tel_dst << ")\n";

    string src(tel_src);
    string dst(tel_dst);
    if (src.size() > jnp1::TEL_NUM_MAX_LEN|| dst.size() > jnp1::TEL_NUM_MAX_LEN) {
        cerr << "maptel: maptel_insert: tel is too long\n";
        assert(false);

    }

    if (!correct_tel_chars(src) || !correct_tel_chars(dst)) {
        cerr << "maptel: maptel_insert: tel is incorrect\n";
        assert(false);
    }




}


static inline void maptel_postinsert_debug(unsigned long id, char const *tel_src, char const *tel_dst) {
    if (maps()[id].find(tel_src) == maps()[id].end()) {
        cerr << "maptel: maptel_insert: failed to add member " << tel_src << " to map " << id << "\n";
        assert(false);
    }

    if (maps()[id][tel_src] != tel_dst) {
        cerr << "maptel: maptel_insert: inserted incorrect tel_dst for member " << tel_src << "\n";
        assert(false);
    }

    cerr << "maptel: maptel_insert: inserted\n";
}


void jnp1::maptel_insert(unsigned long id, char const *tel_src, char const *tel_dst) {
    if (debug) {
        maptel_preinsert_debug(id, tel_src, tel_dst);
    }
    string src(tel_src);
    string dst(tel_dst);
    maps()[id][src] = dst;

    if (debug) {
        maptel_postinsert_debug(id, tel_src, tel_dst);
    }
}


static inline void maptel_preerase_debug(unsigned long id, char const *tel_src, bool *tel_src_present) {
    if (tel_src == NULL) {
        cerr << "maptel: maptel_erase: pointer is null\n";
        assert(false);
    }

    cerr << "maptel: maptel_erase(" << id << ", " << tel_src << ")\n";

    if (maps().find(id) == maps().end()) {
        cerr << "maptel: maptel_erase: nothing to erase\n";
        assert(false);
    }

    if (strcmp(tel_src, "") == 0) {
        cerr << "maptel: maptel_delete: tel_src is empty\n";
        assert(false);
    }

    string src(tel_src);
    if (src.size() > jnp1::TEL_NUM_MAX_LEN) {
        cerr << "maptel: maptel_erase: tel is too long\n";
        assert(false);
    }

    if (!correct_tel_chars(src)) {
        cerr << "maptel: maptel_erase: tel is incorrect\n";
        assert(false);
    }

    if (maps()[id].find(src) != maps()[id].end()) {
        *tel_src_present = true;
    }
}


static inline void maptel_posterase_debug(unsigned long id, char const *tel_src, bool const tel_src_present) {
    if (maps()[id].find(tel_src) != maps()[id].end()) {
        cerr << "maptel: maptel_erase: failed to erase tel " << tel_src << " from map " << id << "\n";
        assert(false);
    }

    if (tel_src_present) {
        cerr << "maptel: maptel_erase: erased\n";
    }
    else {
        cerr << "maptel: maptel_erase: nothing to erase\n";
    }
}


void jnp1::maptel_erase(unsigned long id, char const *tel_src) {
    bool tel_src_present = false;
    if (debug) {
        maptel_preerase_debug(id, tel_src, &tel_src_present);
    }
    string x(tel_src);
    maps()[id].erase(x);

    if (debug) {
        maptel_posterase_debug(id, tel_src, tel_src_present);
    }
}


static inline void maptel_pretransform_debug(unsigned long id, char const *tel_src, char const *tel_dst, size_t len) {
    if (tel_src == NULL) {
        cerr << "maptel: maptel_transform: pointer is null\n";
        assert(false);
    }

    if (tel_dst == NULL) {
        cerr << "maptel: maptel_transform: tel_dst is null\n";
        assert(false);
    }

    if (strcmp(tel_src, "") == 0) {
        cerr << "maptel: maptel_transform: tel_src is empty\n";
        assert(false);
    }

    cerr << "maptel: maptel_transform(" << id << ", " << tel_src << ", " << (void*)tel_dst << ", " << len << ")\n";
    if (maps().find(id) == maps().end()) {
        cerr << "maptel: maptel_transform: map " << id << " doesn't exists\n";
        assert(false);
    }

    string src(tel_src);
    if (src.size() > jnp1::TEL_NUM_MAX_LEN) {
        cerr << "maptel: maptel_transform: tel is too long\n";
        assert(false);
    }

    if (!correct_tel_chars(src)) {
        cerr << "maptel: maptel_transform: tel is incorrect\n";
        assert(false);
    }
}


static inline void maptel_transform_len_check(char const *tel_src, size_t len) {
    if (strlen(tel_src) >= len) {
        cerr << "maptel: maptel_transform: len is too small\n";
        assert(false);
    }
}


static inline void maptel_posttransform_debug(char const *tel_src, char *tel_dst, char const *func_result, bool const cycle_detected) {
    if (strcmp(func_result, tel_dst) != 0) {
        cerr << "maptel: maptel_transform: failed to pass transform result to tel_dst\n";
        assert(false);
    }

    if (cycle_detected) {
        cerr << "maptel: maptel_transform: " << tel_src << " -> " << tel_src << "\n";
    }
    else {
        cerr << "maptel: maptel_transform: " << tel_src << " -> " << func_result << "\n";
    }
}


void jnp1::maptel_transform(unsigned long id, char const *tel_src, char *tel_dst, size_t len) {
    if (debug) {
        maptel_pretransform_debug(id, tel_src, tel_dst, len);
    }

    string src(tel_src);
    bool cycle_detected = false;
    unordered_set<string> visited;
    visited.insert(src);

    auto curr_map = maps()[id];
    auto it = curr_map.find(src);

    while (it != curr_map.end()) {
        string dst = it->second;

        if (visited.find(dst) != visited.end()) {
            cycle_detected = true;
            src = tel_src;
            break;
        }
        else {
            visited.insert(dst);
            src = dst;
            it = curr_map.find(src);
        }
    }

    auto result = src.c_str();
    if (debug) {
        if (cycle_detected) {
            cerr << "maptel: maptel_transform: cycle detected\n";
        }
        maptel_transform_len_check(result, len);
    }
        strcpy(tel_dst, result);

    if (debug) {
        maptel_posttransform_debug(tel_src, tel_dst, result, cycle_detected);
    }
}
