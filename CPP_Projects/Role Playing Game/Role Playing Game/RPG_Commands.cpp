#include "RPG_Commands.h"
#include "RPG_Engine.h"

RPG_Engine* cCommand::g_engine = nullptr;

cScriptProcessor::cScriptProcessor() {
	bUserControlEnabled = true;
}

void cScriptProcessor::AddCommand(cCommand* cmd) {
	m_listCommands.push_back(cmd);
}

void cScriptProcessor::ProcessCommands(float fElapsedTime) {
	// If commands are available, halt user control
	bUserControlEnabled = m_listCommands.empty();

	if (!m_listCommands.empty()) {
		// A command is available
		if (!m_listCommands.front()->bCompleted) {
			// Command has not been started
			if (!m_listCommands.front()->bStarted) {
				m_listCommands.front()->Start();
				m_listCommands.front()->bStarted = true;
			}
			else // Command has been started to process it
				m_listCommands.front()->Update(fElapsedTime);
		}
		else {
			//Command has been completed
			delete m_listCommands.front();
			m_listCommands.pop_front();
		}
	}
}

// Marks Currently active command as complete, from external source
void cScriptProcessor::CompleteCommand() {
	if (!m_listCommands.empty()) {
		m_listCommands.front()->bCompleted = true;
	}
}

cCommand_MoveTo::cCommand_MoveTo(cDynamic* object, float x, float y, float duration) {
	m_fTargetPosX = x;
	m_fTargetPosY = y;
	m_fTimeSoFar = 0.0f;
	m_fDuration = max(duration, 0.001f);
	m_pObject = object;
}

void cCommand_MoveTo::Start() {
	m_fStartPosX = m_pObject->px;
	m_fStartPosY = m_pObject->py;
}

void cCommand_MoveTo::Update(float fElapsedTime) {
	m_fTimeSoFar += fElapsedTime;
	float t = m_fTimeSoFar / m_fDuration;
	if (t > 1.0f) t = 1.0f;

	m_pObject->px = (m_fTargetPosX - m_fStartPosX) * t + m_fStartPosX;
	m_pObject->py = (m_fTargetPosY - m_fStartPosY) * t + m_fStartPosY;
	m_pObject->vx = (m_fTargetPosX - m_fStartPosX) / m_fDuration;
	m_pObject->vy = (m_fTargetPosY - m_fStartPosY) / m_fDuration;

	if (m_fTimeSoFar >= m_fDuration) {
		// Object has reached destination, so stop
		m_pObject->px = m_fTargetPosX;
		m_pObject->py = m_fTargetPosY;
		m_pObject->vx = 0.0f;
		m_pObject->vy = 0.0f;
		bCompleted = true;
	}
}


cCommand_ShowDialog::cCommand_ShowDialog(vector<string> line) {
	vecLines = line;
}

void cCommand_ShowDialog::Start() {
	g_engine->ShowDialog(vecLines);
}


cCommand_ChangeMap::cCommand_ChangeMap(string mapName, float mapPosX, float mapPosY) {
	m_sMapName = mapName;
	m_fMapPosX = mapPosX;
	m_fMapPosY = mapPosY;
}

void cCommand_ChangeMap::Start() {
	g_engine->ChangeMap(m_sMapName, m_fMapPosX, m_fMapPosY);
	bCompleted = true;
}

cCommand_AddQuest::cCommand_AddQuest(cQuest* quest) {
	m_quest = quest;
}

void cCommand_AddQuest::Start() {
	g_engine->AddQuest(m_quest);
	bCompleted = true;
}

cCommand_UpdateQuestPhase::cCommand_UpdateQuestPhase(string questName, int phase) {
	s_questName = questName;
	m_nPhase = phase;
}

void cCommand_UpdateQuestPhase::Start() {
	g_engine->UpdateQuestPhase(s_questName, m_nPhase);
	bCompleted = true;
}

cCommand_GiveItem::cCommand_GiveItem(cItem* item) {
	m_item = item;
}

void cCommand_GiveItem::Start() {
	g_engine->GiveItem(m_item);
	bCompleted = true;
}

cCommand_ChangeCurrentQuest::cCommand_ChangeCurrentQuest(string questName) {
	s_questName = questName;
}

void cCommand_ChangeCurrentQuest::Start() {
	g_engine->ChangeCurrentQuest(s_questName);
	bCompleted = true;
}

cCommand_UpdateCurrentMap::cCommand_UpdateCurrentMap(string mapName) {
	s_mapName = mapName;
}

void cCommand_UpdateCurrentMap::Start() {
	g_engine->UpdateCurrentMap(s_mapName);
	bCompleted = true;
}

void cCommand_UpdateMapDynamics::Start() {
	g_engine->UpdateMapDynamics();
	bCompleted = true;
}

cCommand_UpdateQuestDynamics::cCommand_UpdateQuestDynamics(string questName) {
	s_questName = questName;
}

void cCommand_UpdateQuestDynamics::Start() {
	g_engine->UpdateQuestDynamics(s_questName);
	bCompleted = true;
}