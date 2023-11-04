using Sharpmake;
using EternalEngine;

namespace ReShieldProject
{
	[Sharpmake.Generate]
	public class ReShieldProjectProject : EternalEngineProject
	{
		public ReShieldProjectProject()
			: base()
		{
			Name = "ReShieldProject";
		}

		public override void ConfigureAll(Configuration InConfiguration, Target InTarget)
		{
			base.ConfigureAll(InConfiguration, InTarget);

			InConfiguration.Output = Configuration.OutputType.Exe;

			// Options
			InConfiguration.Options.Add(Options.Vc.Compiler.CppLanguageStandard.CPP14);
			InConfiguration.Options.Add(new Options.Vc.Linker.StackSize(8388608));
			InConfiguration.Options.Add(Options.Vc.General.WarningLevel.Level4);
			InConfiguration.Options.Add(Options.Vc.Compiler.MultiProcessorCompilation.Enable);

			// Library paths
			InConfiguration.IncludePaths.Add(new string[] {
				@"$(SolutionDir)ReShieldProject",
				@"$(SolutionDir)eternal-engine-core\include",
				@"$(SolutionDir)eternal-engine-graphics\include",
				@"$(SolutionDir)eternal-engine-components\include",
				@"$(SolutionDir)eternal-engine-extern\include",
				@"$(SolutionDir)eternal-engine-extern\imgui",
				@"$(SolutionDir)eternal-engine-shaders",
			});

			InConfiguration.ForcedIncludes.Add(new string[] {
				@"ReShield.hpp",
			});

			// Libraries
			InConfiguration.LibraryFiles.Add(new string[] {
				"Xinput9_1_0.lib",
				"Shlwapi.lib",
				"d3d12.lib",
				"d3d11.lib",
				"d3d10.lib",
				"DXGI.lib",
				"dxguid.lib",
				"eternal-engine-extern.lib",
				"eternal-engine-core.lib",
				"eternal-engine-components.lib",
				"eternal-engine-graphics.lib",
				"eternal-engine-utils.lib",
				"libfbxsdk.lib",
				"kernel32.lib",
				"user32.lib",
				"gdi32.lib",
				"winspool.lib",
				"comdlg32.lib",
				"advapi32.lib",
				"shell32.lib",
				"ole32.lib",
				"oleaut32.lib",
				"uuid.lib",
				"odbc32.lib",
				"odbccp32.lib",
				"vulkan-1.lib",
				"shaderc_shared.lib",
				"dxcompiler.lib",
			});

			// Defines
			InConfiguration.Defines.Add(new string[] {
				"SHADERC_ENABLE_SHARED_CRT=ON",
				"FBXSDK_SHARED=1",
			});

			if (InTarget.Optimization == Optimization.Debug)
			{
				InConfiguration.LibraryPaths.Add(new string[] {
					EternalEngineSettings.VulkanPath + @"\Lib",
					EternalEngineSettings.FBXSDKPath + @"\lib\vs2022\x64\debug",
					@"$(SolutionDir)eternal-engine-extern\dxc\lib\x64"
				});

				InConfiguration.TargetCopyFiles.Add(new string[] {
					@"[project.SharpmakeCsPath]\..\eternal-engine-extern\dxc\bin\dxil.dll",
					EternalEngineSettings.FBXSDKPath + @"\lib\vs2022\x64\debug\libfbxsdk.dll",
				});
			}

			if (InTarget.Platform == Platform.win32 || InTarget.Platform == Platform.win64)
			{
				InConfiguration.Options.Add(Options.Vc.Linker.SubSystem.Windows);
				InConfiguration.IncludePaths.Add(new string[] {
					@"$(SolutionDir)packages\Microsoft.Direct3D.D3D12." + EternalEngineSettings.MicrosoftDirect3DD3D12Version + @"\Include",
					@"$(SolutionDir)packages\WinPixEventRuntime." + EternalEngineSettings.WinPixEventRuntimeVersion + @"\Include",
				});
				InConfiguration.ReferencesByNuGetPackage.Add("WinPixEventRuntime", EternalEngineSettings.WinPixEventRuntimeVersion);
				InConfiguration.ReferencesByNuGetPackage.Add("Microsoft.Direct3D.D3D12", EternalEngineSettings.MicrosoftDirect3DD3D12Version);
			}

			InConfiguration.AddPublicDependency<EternalEngineComponentsProject>(InTarget);
			InConfiguration.AddPublicDependency<EternalEngineCoreProject>(InTarget);
			InConfiguration.AddPublicDependency<EternalEngineExternProject>(InTarget);
			InConfiguration.AddPublicDependency<EternalEngineGraphicsProject>(InTarget);
			InConfiguration.AddPrivateDependency<EternalEngineShadersProject>(InTarget);
			InConfiguration.AddPublicDependency<EternalEngineUtilsProject>(InTarget);
		}
	}
}
