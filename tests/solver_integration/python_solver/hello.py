import CoSimIO
return_info = CoSimIO.Hello()

print(return_info)

major_version = return_info.GetInt("major_version")
minor_version = return_info.GetInt("minor_version")
patch_version = return_info.GetString("patch_version")

print("CoSimIO version", str(major_version)+"."+str(minor_version)+"."+patch_version)