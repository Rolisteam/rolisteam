function Component()
{
    Component.prototype.createOperations = function()
    {
        component.createOperations();
        if (systemInfo.productType === "windows") {
            component.addOperation("CreateShortcut", "@TargetDir@/rolisteam.exe", "@StartMenuDir@/rolisteam.lnk",
                                   "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/logo/rolisteam.ico");
            component.addOperation("CreateShortcut", "@TargetDir@/rolisteam.exe", "@DesktopDir@/rolisteam.lnk",
                                   "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/logo/rolisteam.ico");
        }
    }
}
