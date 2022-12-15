#include "pch.h"
#include "Item.h"
void Item::Set()
{
	// ������ ������ �����ش�.
	Protocol::PBVector f ;
	f.set_x(1);
	f.set_y(0);
	f.set_z(0);

	_objInfo.set_allocated_forwarddir(new Protocol::PBVector(YHUtils::RotateVectorPB(f, rand() % 360)));
}

void Item::ItemUpdate()
{
	if (!_isGround)
	{
		Protocol::PBVector location = _objInfo.location();
		_objInfo.set_allocated_location(new Protocol::PBVector(YHUtils::CalParabolaPB(location, _objInfo.forwarddir(), 150 * _deltaTime, _time * 6, GRAVITY)));

		// ����ü�� ���� ��Ҵ��� üũ�ϴ� ����
		if (_objInfo.location().z() <= 0)
		{
			_isGround = true;
			SetLocation(Vector3(location.x(), location.y(), 0));
			return;
		}
		// ���� ��Ŷ ����
		Protocol::S_MOVE movePkt;
		movePkt.set_allocated_objectinfo(new Protocol::ObjectInfo(_objInfo));
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(movePkt);
		GRoom->DoAsync(&Room::Broadcast, sendBuffer);

		GRoom->DoTimer(shared_from_this(), 50, &GameObject::Update);
	}
}

void Item::Update()
{
	GameObject::Update();

	ItemUpdate();
	
}

void Item::State_Dead()
{
	// �÷��̾ �������� ȹ��������

}
