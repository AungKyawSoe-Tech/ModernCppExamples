#include <iostream>

#include <string>

#include <vector>

#include <map>

#include <tuple>

 

using State  = std::tuple<double, double, double, double, double>;

using ParticleWeight = double;

using Particle = std::map<ParticleWeight, State>;

using Particles= std::vector<Particle>;

 

std::size_t Foo(Particles& particlesIn){

    ParticleWeight weightsTotal = 0.0;

 

    for(const auto& particle : particlesIn){

        for (const auto& [weight, state] : particle) {

            weightsTotal +=weight; // 5 + 6 + 7 + 8

        }

    }

    return weightsTotal;

}

 

int main() {

    Particles myParticles{};

    for(int i = 0; i < 4; i++) {

        State state =  std::make_tuple(i, i+1, i+1, i+1, i+1);

        Particle newParticle{};

        newParticle.emplace(5.0 + i, state);

        myParticles.push_back(newParticle);

    }

    std::cout << std::to_string(Foo(myParticles)) << std::endl;

}