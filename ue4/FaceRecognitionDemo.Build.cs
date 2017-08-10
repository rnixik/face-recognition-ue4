// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;
public class FaceRecognitionDemo : ModuleRules
{
    public string ProjectRoot
    {
        get
        {
            return System.IO.Path.GetFullPath(
                System.IO.Path.Combine(ModuleDirectory, "../../")
            );
        }
    }

    private string ThirdPartyPath
	{
		get { return Path.GetFullPath( Path.Combine(ProjectRoot, "ThirdParty/" ) ); }
	}
		
	public FaceRecognitionDemo(ReadOnlyTargetRules Target) : base(Target)
	{
        
        
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "RHI", "RenderCore", "ShaderCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "FaceRecognition", "Includes"));
       
        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "FaceRecognition", "Win64", "FaceRecognition.lib"));
        }
        else
        {
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "FaceRecognition", "Android", "libfacerecognition.so"));
        }
	}
}
