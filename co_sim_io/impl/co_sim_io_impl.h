// Kratos   ______     _____ _           ________
//         / ____/___ / ___/(_)___ ___  /  _/ __ |
//        / /   / __ \\__ \/ / __ `__ \ / // / / /
//       / /___/ /_/ /__/ / / / / / / // // /_/ /
//       \____/\____/____/_/_/ /_/ /_/___/\____/
//      Kratos CoSimulationApplication
//
//  License:         BSD License, see license.txt
//
//  Main authors:    Philipp Bucher (https://github.com/philbucher)
//

#ifndef KRATOS_CO_SIM_IO_IMPL_H_INCLUDED
#define KRATOS_CO_SIM_IO_IMPL_H_INCLUDED

/*
This file contains the implementation of the functions defined in "co_sim_io.h"
*/

// System includes
#include <string>
#include <memory>

// Project includes
#include "co_sim_connection.h"

namespace CoSimIO {

namespace Internals {
// TODO make sure this is unique even across compilation units (test somehow)
static std::unordered_map<std::string, std::unique_ptr<CoSimConnection>> s_co_sim_connections;

static bool HasIO(const std::string& rConnectionName)
{
    return s_co_sim_connections.find(rConnectionName) != s_co_sim_connections.end();
}

static CoSimConnection& GetConnection(const std::string& rConnectionName)
{
    KRATOS_CO_SIM_ERROR_IF_NOT(HasIO(rConnectionName)) << "Trying to use connection \"" << rConnectionName << "\" which does not exist!" << std::endl;
    return *s_co_sim_connections.at(rConnectionName);
}

inline void SendControlSignal(
    const std::string& rConnectionName,
    const std::string& rIdentifier,
    const CoSimIO::ControlSignal Signal)
{
    Internals::GetConnection(rConnectionName).SendControlSignal(rIdentifier, Signal);
}

} // namespace Internals

inline ReturnInfo Connect(const std::string& rConnectionName, CoSimIO::SettingsType Settings)
{
    using namespace Internals;
    KRATOS_CO_SIM_ERROR_IF(HasIO(rConnectionName)) << "A connection for \"" << rConnectionName << "\" already exists!" << std::endl;

    s_co_sim_connections[rConnectionName] = std::unique_ptr<CoSimConnection>(new CoSimConnection(rConnectionName, Settings));
    GetConnection(rConnectionName).Connect();
}

inline ReturnInfo Connect(const std::string& rConnectionName, const std::string& rSettingsFileName)
{
    Connect(rConnectionName, Internals::ReadSettingsFile(rSettingsFileName)); // forward call to funciton above
}

inline ReturnInfo Disconnect(const std::string& rConnectionName)
{
    using namespace Internals;
    KRATOS_CO_SIM_ERROR_IF_NOT(HasIO(rConnectionName)) << "Trying to disconnect connection \"" << rConnectionName << "\" which does not exist!" << std::endl;

    GetConnection(rConnectionName).Disconnect();
    s_co_sim_connections.erase(rConnectionName);
}

// Version for C++, there this input is a std::vector, which we have to wrap before passing it on
template<>
inline ReturnInfo ImportData(
    const std::string& rConnectionName,
    const std::string& rIdentifier,
    std::vector<double>& rData)
{
    using namespace CoSimIO::Internals;
    std::unique_ptr<DataContainer<double>> p_container(new DataContainerStdVector<double>(rData));
    GetConnection(rConnectionName).ImportData(rIdentifier, *p_container);
}

// Version for C and fortran, there we already get a container
template<>
inline ReturnInfo ImportData(
    const std::string& rConnectionName,
    const std::string& rIdentifier,
    CoSimIO::Internals::DataContainer<double>& rData)
{
    Internals::GetConnection(rConnectionName).ImportData(rIdentifier, rData);
}

// Version for C++, there this input is a std::vector, which we have to wrap before passing it on
template<>
inline ReturnInfo ExportData(
    const std::string& rConnectionName,
    const std::string& rIdentifier,
    const std::vector<double>& rData)
{
    using namespace CoSimIO::Internals;
    std::unique_ptr<DataContainer<double>> p_container(new DataContainerStdVectorReadOnly<double>(rData));
    GetConnection(rConnectionName).ExportData(rIdentifier, *p_container);
}

// Version for C and fortran, there we already get a container
template<>
inline ReturnInfo ExportData(
    const std::string& rConnectionName,
    const std::string& rIdentifier,
    const CoSimIO::Internals::DataContainer<double>& rData)
{
    Internals::GetConnection(rConnectionName).ExportData(rIdentifier, rData);
}

template<>
inline ReturnInfo ImportMesh(
    const std::string& rConnectionName,
    const std::string& rIdentifier,
    std::vector<double>& rNodalCoordinates,
    std::vector<int>& rElementConnectivities,
    std::vector<int>& rElementTypes)
{
    using namespace CoSimIO::Internals;
    std::unique_ptr<DataContainer<double>> p_container_coords(new DataContainerStdVector<double>(rNodalCoordinates));
    std::unique_ptr<DataContainer<int>> p_container_conn(new DataContainerStdVector<int>(rElementConnectivities));
    std::unique_ptr<DataContainer<int>> p_container_types(new DataContainerStdVector<int>(rElementTypes));
    Internals::GetConnection(rConnectionName).ImportMesh(rIdentifier, *p_container_coords, *p_container_conn, *p_container_types);
}

template<>
inline ReturnInfo ImportMesh(
    const std::string& rConnectionName,
    const std::string& rIdentifier,
    CoSimIO::Internals::DataContainer<double>& rNodalCoordinates,
    CoSimIO::Internals::DataContainer<int>& rElementConnectivities,
    CoSimIO::Internals::DataContainer<int>& rElementTypes)
{
    Internals::GetConnection(rConnectionName).ImportMesh(rIdentifier, rNodalCoordinates, rElementConnectivities, rElementTypes);
}

template<>
inline ReturnInfo ExportMesh(
    const std::string& rConnectionName,
    const std::string& rIdentifier,
    const std::vector<double>& rNodalCoordinates,
    const std::vector<int>& rElementConnectivities,
    const std::vector<int>& rElementTypes)
{
    using namespace CoSimIO::Internals;
    std::unique_ptr<DataContainer<double>> p_container_coords(new DataContainerStdVectorReadOnly<double>(rNodalCoordinates));
    std::unique_ptr<DataContainer<int>> p_container_conn(new DataContainerStdVectorReadOnly<int>(rElementConnectivities));
    std::unique_ptr<DataContainer<int>> p_container_types(new DataContainerStdVectorReadOnly<int>(rElementTypes));
    Internals::GetConnection(rConnectionName).ExportMesh(rIdentifier, *p_container_coords, *p_container_conn, *p_container_types);
}

template<>
inline ReturnInfo ExportMesh(
    const std::string& rConnectionName,
    const std::string& rIdentifier,
    const CoSimIO::Internals::DataContainer<double>& rNodalCoordinates,
    const CoSimIO::Internals::DataContainer<int>& rElementConnectivities,
    const CoSimIO::Internals::DataContainer<int>& rElementTypes)
{
    Internals::GetConnection(rConnectionName).ExportMesh(rIdentifier, rNodalCoordinates, rElementConnectivities, rElementTypes);
}

inline ReturnInfo ImportSolutionInfo(
    const std::string& rConnectionName,
    SolutionInfo& rSolutionInfo)
{
    // Internals::GetConnection(rConnectionName).ImportSolutionInfo(rSolutionInfo);
}

inline ReturnInfo ExportSolutionInfo(
    const std::string& rConnectionName,
    const SolutionInfo& rSolutionInfo)
{
    // Internals::GetConnection(rConnectionName).ExportSolutionInfo(rSolutionInfo);
}

inline int IsConverged(const std::string& rConnectionName)
{
    return Internals::GetConnection(rConnectionName).IsConverged();
}

inline ReturnInfo Run(const std::string& rConnectionName)
{
    Internals::GetConnection(rConnectionName).Run();
}

template<>
inline ReturnInfo Register(
    const std::string& rConnectionName,
    const std::string& rFunctionName,
    std::function<void()> FunctionPointer)
{
    using namespace CoSimIO::Internals;

    auto fct_callback = [FunctionPointer](CoSimConnection& rConnection, const std::string& rIdentifier)
    {
        FunctionPointer();
    };

    Internals::GetConnection(rConnectionName).Register(rFunctionName, fct_callback);
}

template<>
inline ReturnInfo Register(
    const std::string& rConnectionName,
    const std::string& rFunctionName,
    void (*pFunctionPointer)(void))
{
    using namespace CoSimIO::Internals;

    auto fct_callback = [pFunctionPointer](CoSimConnection& rConnection, const std::string& rIdentifier)
    {
        pFunctionPointer();
    };

    Internals::GetConnection(rConnectionName).Register(rFunctionName, fct_callback);
}

template<>
inline ReturnInfo Register(
    const std::string& rConnectionName,
    const std::string& rFunctionName,
    std::function<double(double)> FunctionPointer)
{
    using namespace CoSimIO::Internals;

    auto fct_callback = [FunctionPointer](CoSimConnection& rConnection, const std::string& rIdentifier)
    {
        std::vector<double> time_vec(1);
        std::unique_ptr<DataContainer<double>> p_container_time(new DataContainerStdVector<double>(time_vec));
        rConnection.ImportData("time_from_co_sim", *p_container_time);
        time_vec[0] = FunctionPointer(time_vec[0]);
        rConnection.ExportData("time_to_co_sim", *p_container_time);
    };

    Internals::GetConnection(rConnectionName).Register(rFunctionName, fct_callback);
}

inline ReturnInfo Register(
    const std::string& rConnectionName,
    const std::string& rFunctionName,
    double (*pFunctionPointer)(double))
{
    using namespace CoSimIO::Internals;

    auto fct_callback = [pFunctionPointer](CoSimConnection& rConnection, const std::string& rIdentifier)
    {
        std::vector<double> time_vec(1);
        std::unique_ptr<DataContainer<double>> p_container_time(new DataContainerStdVector<double>(time_vec));
        rConnection.ImportData("time_from_co_sim", *p_container_time);
        time_vec[0] = pFunctionPointer(time_vec[0]);
        rConnection.ExportData("time_to_co_sim", *p_container_time);
    };

    Internals::GetConnection(rConnectionName).Register(rFunctionName, fct_callback);
}

template<>
inline ReturnInfo Register(
    const std::string& rConnectionName,
    const std::string& rFunctionName,
    std::function<void(const std::string&, const std::string&)> FunctionPointer)
{
    using namespace CoSimIO::Internals;

    auto fct_callback = [FunctionPointer](CoSimConnection& rConnection, const std::string& rIdentifier)
    {
        FunctionPointer(rConnection.GetConnectionName(), rIdentifier);
    };

    Internals::GetConnection(rConnectionName).Register(rFunctionName, fct_callback);
}

template<>
inline ReturnInfo Register(
    const std::string& rConnectionName,
    const std::string& rFunctionName,
    void (*pFunctionPointer)(const std::string&, const std::string&))
{
    using namespace CoSimIO::Internals;

    auto fct_callback = [pFunctionPointer](CoSimConnection& rConnection, const std::string& rIdentifier)
    {
        pFunctionPointer(rConnection.GetConnectionName(), rIdentifier);
    };

    Internals::GetConnection(rConnectionName).Register(rFunctionName, fct_callback);
}

template<>
inline ReturnInfo Register(
    const std::string& rConnectionName,
    const std::string& rFunctionName,
    void (*pFunctionPointer)(const char*, const char*))
{
    using namespace CoSimIO::Internals;

    auto fct_callback = [pFunctionPointer](CoSimConnection& rConnection, const std::string& rIdentifier)
    {
        pFunctionPointer(rConnection.GetConnectionName().c_str(), rIdentifier.c_str());
    };

    Internals::GetConnection(rConnectionName).Register(rFunctionName, fct_callback);
}

} // namespace CoSimIO

#endif // KRATOS_CO_SIM_IO_IMPL_H_INCLUDED
