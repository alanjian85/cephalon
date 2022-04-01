#ifndef CEPHALON_TERRAINS_TERRAIN_H_
#define CEPHALON_TERRAINS_TERRAIN_H_

#include <map>
#include <string>

#include "world/chunk.h"

namespace cephalon {
    class Terrain {
    public:
        static Terrain* getTerrain(const std::string& name) {
            auto it = terrains_.find(name);
            if (it != terrains_.cend())
                return it->second;
            return nullptr;
        }

        Terrain(const std::string& name) {
            terrains_[name] = this;
        }

        virtual ~Terrain() noexcept = default;

        virtual void setSeed(unsigned seed) = 0;

        virtual void genChunk(Chunk& chunk) const = 0;
    private:
        static std::map<std::string, Terrain*> terrains_;
    };
}

#include "normal.h"

namespace cephalon {
    namespace terrains {
        extern Normal normal;
    }
}

#endif // CEPHALON_TERRAINS_TERRAIN_H_