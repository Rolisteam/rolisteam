function Component()
{
    Component.prototype.createOperations = function()
    {
        component.createOperations();
        if (systemInfo.productType === "windows") {
            component.addOperation("CreateShortcut", "@TargetDir@/dice.exe", "@StartMenuDir@/dice.lnk",
                                   "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/logo/dice.ico");
            component.addOperation("CreateShortcut", "@TargetDir@/dice.exe", "@DesktopDir@/dice.lnk",
                                   "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/logo/dice.ico");
        }
    }
}
