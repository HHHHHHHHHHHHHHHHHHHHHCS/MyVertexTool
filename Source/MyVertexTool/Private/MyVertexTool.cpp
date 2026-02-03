#include "MyVertexTool.h"

static const FName MyVertexToolTabName("MyVertexTool");

#define LOCTEXT_NAMESPACE "FMyVertexToolModule"

void FMyVertexToolModule::StartupModule()
{
}

void FMyVertexToolModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMyVertexToolModule, MyVertexTool)
