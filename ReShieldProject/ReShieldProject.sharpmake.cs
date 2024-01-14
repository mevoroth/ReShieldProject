using Sharpmake;
using EternalEngine;

namespace ReShieldProject
{
	[Sharpmake.Generate]
	public class ReShieldProjectProject : EternalEngineBaseProject
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
			InConfiguration.TargetFileFullExtension = null;


			// Include paths
			InConfiguration.IncludePaths.AddRange(new string[] {
				@"$(SolutionDir)ReShieldProject",
				@"$(SolutionDir)eternal-engine-core\include",
				@"$(SolutionDir)eternal-engine-graphics\include",
				@"$(SolutionDir)eternal-engine-components\include",
				@"$(SolutionDir)eternal-engine-extern\include",
				@"$(SolutionDir)eternal-engine-extern\imgui",
				@"$(SolutionDir)eternal-engine-shaders",
			});

			if (!ExtensionMethods.IsPC(InTarget.Platform))
			{
				InConfiguration.IncludePaths.AddRange(new string[] {
					@"$(SolutionDir)ReShieldProject",
					@"$(SolutionDir)eternal-engine-core\CorePrivate\include",
					@"$(SolutionDir)eternal-engine-graphics\GraphicsPrivate\include",
				});
			}

			InConfiguration.ForcedIncludes.AddRange(new string[] {
				@"ReShield.hpp",
			});

			// Libraries
			if (ExtensionMethods.IsPC(InTarget.Platform))
			{
				InConfiguration.LibraryFiles.AddRange(new string[] {
					"Xinput9_1_0.lib",
					"Shlwapi.lib",
					"d3d12.lib",
					"DXGI.lib",
					"dxguid.lib",
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
			}

			// Defines
			InConfiguration.Defines.AddRange(new string[] {
				"SHADERC_ENABLE_SHARED_CRT=ON",
				"FBXSDK_SHARED=1",
			});

			if (InTarget.Optimization == Optimization.Debug)
			{
				InConfiguration.LibraryPaths.AddRange(new string[] {
					EternalEngineSettings.VulkanPath + @"\Lib",
					EternalEngineSettings.FBXSDKPath + @"\lib\vs2022\x64\debug",
					@"$(SolutionDir)eternal-engine-extern\dxc\lib\x64"
				});

				InConfiguration.TargetCopyFiles.AddRange(new string[] {
					@"[project.SharpmakeCsPath]\..\eternal-engine-extern\dxc\bin\dxil.dll",
					EternalEngineSettings.FBXSDKPath + @"\lib\vs2022\x64\debug\libfbxsdk.dll",
				});
			}

			if (ExtensionMethods.IsPC(InTarget.Platform))
			{
				InConfiguration.Options.Add(Options.Vc.Linker.SubSystem.Windows);

				if (InTarget.Optimization == Optimization.Debug)
				{
					InConfiguration.IncludePaths.AddRange(new string[] {
						@"$(SolutionDir)packages\Microsoft.Direct3D.D3D12." + EternalEngineSettings.MicrosoftDirect3DD3D12Version + @"\Include",
						@"$(SolutionDir)packages\WinPixEventRuntime." + EternalEngineSettings.WinPixEventRuntimeVersion + @"\Include",
					});
					InConfiguration.LibraryPaths.AddRange(new string[] {
						@"$(SolutionDir)packages\WinPixEventRuntime." + EternalEngineSettings.WinPixEventRuntimeVersion + @"\bin\x64",
					});
					InConfiguration.LibraryFiles.AddRange(new string[] {
						"WinPixEventRuntime.lib"
					});
					InConfiguration.ReferencesByNuGetPackage.Add("WinPixEventRuntime", EternalEngineSettings.WinPixEventRuntimeVersion);
					InConfiguration.ReferencesByNuGetPackage.Add("Microsoft.Direct3D.D3D12", EternalEngineSettings.MicrosoftDirect3DD3D12Version);

					InConfiguration.TargetCopyFiles.AddRange(new string[] {
						@"[project.SharpmakeCsPath]\..\packages\WinPixEventRuntime." + EternalEngineSettings.WinPixEventRuntimeVersion + @"\bin\x64\WinPixEventRuntime.dll"
					});
				}
			}

			InConfiguration.SourceFilesBuildExcludeRegex.Add(ExtensionMethods.IsPC(InTarget.Platform) ? @".*\\main.cpp$" : @".*\\WinMain.cpp$");

			InConfiguration.AddPublicDependency<EternalEngineComponentsProject>(InTarget);
			InConfiguration.AddPublicDependency<EternalEngineCoreProject>(InTarget);
			InConfiguration.AddPublicDependency<EternalEngineExternProject>(InTarget);
			InConfiguration.AddPublicDependency<EternalEngineGraphicsProject>(InTarget);
			InConfiguration.AddPrivateDependency<EternalEngineShadersProject>(InTarget);
			InConfiguration.AddPublicDependency<EternalEngineUtilsProject>(InTarget);
		}
	}
}
