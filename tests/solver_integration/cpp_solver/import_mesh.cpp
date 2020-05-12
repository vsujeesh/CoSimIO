//     ______     _____ _           ________
//    / ____/___ / ___/(_)___ ___  /  _/ __ |
//   / /   / __ \\__ \/ / __ `__ \ / // / / /
//  / /___/ /_/ /__/ / / / / / / // // /_/ /
//  \____/\____/____/_/_/ /_/ /_/___/\____/
//  Kratos CoSimulationApplication
//
//  License:         BSD License, see license.txt
//
//  Main authors:    Pooyan Dadvand
//

// CoSimulation includes
#include "co_sim_io.hpp"

#define COSIMIO_CHECK_EQUAL(a, b)                                \
    if (a != b) {                                                \
        std::cout << "in line " << __LINE__ << " : " << a        \
                  << " is not equal to " << b << std::endl;      \
        return 1;                                                \
    }

int main()
{
    CoSimIO::ConnectionSettings settings;
    settings.Set("connection_name", "test_connection"); // This must be unique for each connection between two solvers
    settings.Set("solver_name", "solver_1"); // Not to be confused with the connection name.
    settings.Set("echo_level", 1);
    settings.Set("solver_version", "1.25");

    auto return_info = CoSimIO::Connect(settings);
    COSIMIO_CHECK_EQUAL(return_info.Get<int>("connection_status"), CoSimIO::ConnectionStatus::Connected);

    std::vector<double> expected_nodal_coordinates{
        0.0, 2.5, 1.0, /*0*/
        2.0, 0.0, 1.5, /*1*/
        2.0, 2.5, 1.5, /*2*/
        4.0, 2.5, 1.7, /*3*/
        4.0, 0.0, 1.7, /*4*/
        6.0, 0.0, 1.8  /*5*/
    };

    std::vector<int> expected_elements_connectivities = {
        0, 1, 2, /*1*/
        1, 3, 2, /*2*/
        1, 4, 3, /*3*/
        3, 4, 5, /*4*/
    };

    std::vector<int> expected_elements_types = {5,5,5,5}; // VTK_TRIANGLE

    CoSimIO::Info info;
    info.Set("identifier", "vector_of_pi");
    info.Set("connection_name", "test_connection");

    std::vector<double> nodal_coordinates;
    std::vector<int> elements_connectivities;
    std::vector<int> elements_types;

    return_info = CoSimIO::ImportMesh(info,nodal_coordinates, elements_connectivities, elements_types);

    COSIMIO_CHECK_EQUAL(nodal_coordinates.size(), expected_nodal_coordinates.size());
    COSIMIO_CHECK_EQUAL(elements_connectivities.size(), expected_elements_connectivities.size());
    COSIMIO_CHECK_EQUAL(elements_types.size(), elements_types.size());

    for(std::size_t i = 0 ; i < nodal_coordinates.size() ; i++)
        COSIMIO_CHECK_EQUAL(nodal_coordinates[i], expected_nodal_coordinates[i])
    for(std::size_t i = 0 ; i < elements_connectivities.size() ; i++)
        COSIMIO_CHECK_EQUAL(elements_connectivities[i], expected_elements_connectivities[i])
    for(std::size_t i = 0 ; i < elements_types.size() ; i++)
        COSIMIO_CHECK_EQUAL(elements_types[i], expected_elements_types[i])

    return_info = CoSimIO::Disconnect(settings); // disconnect afterwards
    COSIMIO_CHECK_EQUAL(return_info.Get<int>("connection_status"), CoSimIO::ConnectionStatus::Disconnected);

    return 0;
}