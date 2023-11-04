function Component()
{
    Component.prototype.createOperations = function()
    {
        component.createOperations();
        if (systemInfo.productType === "windows") {
            component.addOperation("CreateShortcut", "@TargetDir@/mindmap.exe", "@StartMenuDir@/mindmap.lnk",
                                   "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/logo/mindmap.ico");
            component.addOperation("CreateShortcut", "@TargetDir@/mindmap.exe", "@DesktopDir@/mindmap.lnk",
                                   "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/logo/mindmap.ico");
        }
    }
}
