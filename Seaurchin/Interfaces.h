#pragma once

#define SU_IF_KEY "Key"

class ExecutionManager;
void InterfacesRegisterEnum(ExecutionManager *exm);
void InterfacesRegisterGlobalFunction(ExecutionManager *exm);
void InterfacesRegisterSceneFunction(ExecutionManager *exm);
void InterfacesExitApplication();