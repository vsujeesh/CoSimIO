import CoSimIO

def cosimio_check_equal(a, b):
    assert a == b


settings = CoSimIO.ConnectionSettings()
settings.SetString("connection_name", "test_connection") # This must be unique for each connection between two solvers
settings.SetString("solver_name", "my_solver") # Not to be confused with the connection name. 
settings.SetInt("echo_level", 1)
settings.SetString("solver_version", "1.25")

return_info = CoSimIO.Connect(settings)
cosimio_check_equal(return_info.GetInt("connection_status"), CoSimIO.ConnectionStatus.Connected)

return_info = CoSimIO.Disconnect(settings)
cosimio_check_equal(return_info.GetInt("connection_status"), CoSimIO.ConnectionStatus.Disconnected)

