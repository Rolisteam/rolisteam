## Dockerfile for building a roliserver container which can be used to host multiple games. 

Copy the docker file to a build directory and run the following.

    docker build -t rolisteam:debian .

The resulting image can be then run as:

    docker run -d  --name rolisteam-server -p 6660:6660 rolisteam:debian


The default config file will be used which uses the default port of 6660 and default blank password for the Admin user.

This are exported to `/opt/default.config` and run against. 

    [General]
    AdminPassword=@Invalid()
    ChannelCount=1
    ConnectionMax=50
    DeepInspectionLog=false
    IpBan=
    IpMode=
    IpRange=
    LogFile=
    LogLevel=1
    MaxMemorySize=@Invalid()
    ServerPassword=
    ThreadCount=1
    TimeEnd=
    TimeStart=
    TimeToRetry=100
    TryCount=10
    port=6660



These can be changed after image creation by logging into the container and editing the file.

    docker exec -it rolisteam-server /bin/bash
