function Component()
{
    Component.prototype.createOperations = function()
    {
        component.createOperations();
        if (systemInfo.productType === "windows") {
            component.addOperation("CreateShortcut", "@TargetDir@/rcse.exe", "@StartMenuDir@/rcse.lnk",
                                   "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/logo/rcse.ico");
            component.addOperation("CreateShortcut", "@TargetDir@/rcse.exe", "@DesktopDir@/rcse.lnk",
                                   "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/logo/rcse.ico");
        }
    }
}
