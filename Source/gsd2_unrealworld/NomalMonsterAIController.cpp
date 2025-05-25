
#include "NomalMonsterAIController.h"
ANomalMonsterAIController::ANomalMonsterAIController(){
	//AI Perception �� ����
	SightConfig->SightRadius = 1000.f; // �⺻ Ž�� �Ÿ�
	SightConfig->LoseSightRadius = 1500.f; //�÷��̾ ��ġ�� �Ÿ� 
	SightConfig->PeripheralVisionAngleDegrees = 100.f; // �þ߰�
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 300.f; //�þ߿��� ��� ��� �Ŀ��� �����ϴ� �Ÿ�

	AIPerceptionComponent->ConfigureSense(*SightConfig); //������ ������ AI Perception ����
	DetectionDistance = 500.f; //AI perception�� Ž�� ���������� ���Ͱ� �÷��̾ �����ϴ� �Ÿ�

	PatrolDelay = 3.0f; // ���� ������
	PatrolRadius = 500.f; //���� �ݰ�
}
