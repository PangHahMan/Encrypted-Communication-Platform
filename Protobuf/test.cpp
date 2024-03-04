#include <iostream>
#include <string>
#include "Persion.pb.h"
using namespace std;

/*
	message Persion
	{
		int32 id = 1;   // ��Ŵ�1��ʼ
		string name = 2;
		string sex = 3;
		int32 age = 4;
	}
*/

int main()
{
	// 1. ����persion����, ����ʼ��
	itcast::Persion p;
	p.set_id(1001);
	// ����һ���ڴ�, �洢����
	p.add_name();
	p.set_name(0, "·��");   //set_name() �Ĳ�����0��ʼ����ʾ�����±�

	p.add_name();
	p.set_name(1, "��˹");

	p.add_name();
	p.set_name(2, "����");

	p.set_sex("man");
	p.set_age(17);
	
	// ö��
	p.set_color(itcast::Blue);
	//mutable_info() �����᷵��һ��ָ�� p ��Ϣ�� Info �ֶε�ָ�룬����ͨ�����ָ�����޸� Info ������
	phw::Info* info = p.mutable_info();
	info->set_address("������ƽ��tdb����");
	info->set_number(911);

	// 2. ��persion�������л� -> �ַ���
	string output;
	p.SerializePartialToString(&output);
	cout << "���л��������: " << output << endl;

	// 3. ���ݴ���
	// 4. ��������, ����(output) -> ���� -> ԭʼ����
	// 4.1 ����Persion����
	itcast::Persion pp;
	pp.ParseFromString(output);
	// ��Info����ֵȡ��
	phw::Info ii = pp.info();
	// 5. ����ԭʼ���� -> ��ӡ������Ϣ
	cout << "id: " << pp.id() << ", name: "
		<< pp.name(0) << ", "
		<< pp.name(1) << ", "
		<< pp.name(2)
		<< ", sex: " << pp.sex() << ", age: " << pp.age()
		<< ", color: " << pp.color()
		<< ", address: " << ii.address()
		<< ", number: " << ii.number()
		<< endl;

	return 0;
}