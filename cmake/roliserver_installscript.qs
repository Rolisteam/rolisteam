function Component()
{
    Component.prototype.createOperations = function()
    {
        component.createOperations();
        if (systemInfo.productType === "windows") {
            component.addOperation("CreateShortcut", "@TargetDir@/roliserver.exe", "@StartMenuDir@/roliserver.lnk",
                                   "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/logo/roliserver.ico");
            component.addOperation("CreateShortcut", "@TargetDir@/roliserver.exe", "@DesktopDir@/roliserver.lnk",
                                   "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/logo/roliserver.ico");
        }
    }
}
