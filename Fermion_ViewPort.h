#include"FermionBase.h"

class FermionViewPortClass : public FermionBaseClass
{
public:
	FermionViewPortClass();
	virtual ~FermionViewPortClass();

	bool LoadContent();
	void UnLoadContent();

	void Update(float dt);
	void Render(float ,float, float, float);
};

FermionViewPortClass::FermionViewPortClass()
{

}

FermionViewPortClass::~FermionViewPortClass()
{

}

bool FermionViewPortClass::LoadContent()
{
	return true;
}

void FermionViewPortClass::UnLoadContent()
{

}

void FermionViewPortClass::Update(float dt)
{

}

void FermionViewPortClass::Render(float red, float green, float blue, float alpha)
{
	if(d3dContext_==0)
		return;

	float clearColor[4]={red,green,blue,alpha};

	d3dContext_->ClearRenderTargetView(backBufferTarget_,clearColor);
	swapChain_->Present(0,0);
}