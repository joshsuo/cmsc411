cmake_minimum_required(VERSION 3.10)
project(project3)

set(CMAKE_CXX_STANDARD 11)

include_directories(protocols)
include_directories(sim)

add_executable(project3
        protocols/MESI_protocol.cpp
        protocols/MESI_protocol.h
        protocols/messages.cpp
        protocols/messages.h
        protocols/MI_protocol.cpp
        protocols/MI_protocol.h
        protocols/MOESI_protocol.cpp
        protocols/MOESI_protocol.h
        protocols/MOESIF_protocol.cpp
        protocols/MOESIF_protocol.h
        protocols/MOSI_protocol.cpp
        protocols/MOSI_protocol.h
        protocols/MSI_protocol.cpp
        protocols/MSI_protocol.h
        protocols/protocol.cpp
        protocols/protocol.h
        sim/bus.cpp
        sim/bus.h
        sim/enums.h
        sim/hash_table.cpp
        sim/hash_table.h
        sim/main.cpp
        sim/memory.cpp
        sim/memory.h
        sim/module.cpp
        sim/module.h
        sim/mreq.cpp
        sim/mreq.h
        sim/node.cpp
        sim/node.h
        sim/preq.cpp
        sim/processor.cpp
        sim/processor.h
        sim/settings.cpp
        sim/settings.h
        sim/sharers.cpp
        sim/sharers.h
        sim/sim.cpp
        sim/sim.h
        sim/sim_analysis.cpp
        sim/sim_analysis.h
        sim/types.h)
