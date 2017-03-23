#include<d3d11.h>
#include<d3dx11.h>
#include<DxErr.h>

class FermionBaseClass{

protected:
	//---> Direct 3D Selected Devices and SwapChains
	ID3D11Device *device_;
	ID3D11DeviceContext *d3dContext_;
	IDXGISwapChain *swapChain_;
	//<---

	ID3D11RenderTargetView* backBufferTarget_; // render Target object

	D3D_DRIVER_TYPE driverType_;
	D3D_FEATURE_LEVEL featureLevel_;

	unsigned int creationFlags; // Flags for Swap Chain creation function

	unsigned int totalDrivers;  // Number of total drivers 
	unsigned int totalFeatures; // Number of total  feature levels

	int width;   // width of the screen to render
	int height;	// height of the screen to render

	HWND hwnd_;  // for holding window handle
	HINSTANCE hInstance_;  // instance of the window

	HRESULT result;   // result of Swap Chain creation function

public:
	FermionBaseClass();
	virtual ~FermionBaseClass();
	
	bool initDx11(HWND, HINSTANCE);
	void ShutDown();

	virtual bool LoadContent();
	virtual void UnLoadContent();

	virtual void Update(float dt)=0;
	virtual void Render(float,float,float,float)=0;
};

//--->Constructor
FermionBaseClass::FermionBaseClass(): driverType_(D3D_DRIVER_TYPE_NULL),featureLevel_(D3D_FEATURE_LEVEL_11_0),device_(0),swapChain_(0),d3dContext_(0),backBufferTarget_(0)
{

}
//<-----


//--->Destructor
FermionBaseClass::~FermionBaseClass()
{
	ShutDown();
}
///<----

bool FermionBaseClass::LoadContent()
{
	// can be overriden

	return true;
}

void FermionBaseClass::UnLoadContent()
{

}

void FermionBaseClass::ShutDown()
{
	UnLoadContent();

	if(backBufferTarget_)
		backBufferTarget_->Release();
	if(device_)
		device_->Release();
	if(d3dContext_)
		d3dContext_->Release();
	if(swapChain_)
		swapChain_->Release();

	backBufferTarget_=0;
	device_=0;
	d3dContext_=0;
	swapChain_=0;
}


bool FermionBaseClass::initDx11(HWND hwnd,HINSTANCE hInstance)
{

	hwnd_=hwnd;
	hInstance_=hInstance;

	RECT diamentions;
	GetClientRect(hwnd,&diamentions);

	width=diamentions.right-diamentions.left;
	height=diamentions.bottom-diamentions.top;

	D3D_DRIVER_TYPE driverType[]={D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_SOFTWARE};
	D3D_FEATURE_LEVEL featureLevel[]={D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0};
	
	totalDrivers=ARRAYSIZE(driverType);
	totalFeatures=ARRAYSIZE(featureLevel);

	creationFlags=0;

	//---> Swap Chain Structure Description
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc,sizeof(swapChainDesc));
	swapChainDesc.BufferCount=1;
	swapChainDesc.BufferDesc.Width=width;
	swapChainDesc.BufferDesc.Height=height;
	swapChainDesc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator=60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator=1;
	swapChainDesc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow=hwnd;
	swapChainDesc.Windowed=true;
	swapChainDesc.SampleDesc.Count=1;
	swapChainDesc.SampleDesc.Quality=0;
	//<----

#ifdef _DEBUG
	creationFlags|=D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	for(int i=0;i<totalDrivers;i++)
	{
		result=D3D11CreateDeviceAndSwapChain(0,
											 driverType[i],
											 0,
											 creationFlags, 
											 featureLevel, 
											 totalFeatures, 
											 D3D11_SDK_VERSION, 
											 &swapChainDesc, 
											 &swapChain_, 
											 &device_, 
											 &featureLevel_, 
											 &d3dContext_);

		if(SUCCEEDED(result))
		{
			driverType_=driverType[i];
			break;
		}
	}

	if(FAILED(result))
	{
		DXTRACE_MSG("Cannot Create Device and swapchain");
		return false;
	}

	//---> Render Target Texture Creation 
	ID3D11Texture2D* backBufferTexture; 
	
	result = swapChain_->GetBuffer(0,__uuidof(ID3D11Texture2D),(LPVOID*)&backBufferTexture);  // function to get swap chain back buffer

	if(FAILED(result))
	{
		DXTRACE_MSG("Failed to get swap chain back buffer");
		return false;
	}
	
	result=device_->CreateRenderTargetView(backBufferTexture,0,&backBufferTarget_);  // function to create render target view

	if(backBufferTexture)
		backBufferTexture->Release();

	if(FAILED(result))
	{
		DXTRACE_MSG("Failed to create render target view");
		return false;
	}

	d3dContext_->OMSetRenderTargets(1, &backBufferTarget_,0);	// Setting the render target view
	//<----


	//--->Cretion of view port
	D3D11_VIEWPORT viewPort;
	viewPort.Width=static_cast<float>(width);
	viewPort.Height=static_cast<float>(height);
	viewPort.TopLeftX=0.0f;
	viewPort.TopLeftY=0.0f;
	viewPort.MinDepth=0.0f;
	viewPort.MaxDepth=1.0f;
	d3dContext_->RSSetViewports(1,&viewPort);
	//<----

	return LoadContent();
}