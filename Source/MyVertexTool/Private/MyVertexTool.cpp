#include "MyVertexTool.h"
#include "Interfaces/IPluginManager.h"

static const FName MyVertexToolTabName("MyVertexTool");

#define LOCTEXT_NAMESPACE "FMyVertexToolModule"

void FMyVertexToolModule::StartupModule()
{
	FString pluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("MyVertexTool"))->GetBaseDir(),
										  TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/MyVertexTool"), pluginShaderDir);
}

void FMyVertexToolModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMyVertexToolModule, MyVertexTool)
