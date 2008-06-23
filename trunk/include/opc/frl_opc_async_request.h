#ifndef frl_opc_async_request_h_
#define frl_opc_async_request_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#include <list>
#include <vector>
#include "frl_smart_ptr.h"
#include "frl_exception.h"
#include "os/win32/com/frl_os_win32_com_variant.h"
#include "opc/frl_opc_item_hvqt.h"
#include <boost/noncopyable.hpp>


namespace frl
{
namespace opc
{
class Group;
typedef ComPtr< Group > GroupElem;

class AsyncRequest : private boost::noncopyable
{
private:
	DWORD id;
	DWORD cancelID;
	Bool cancelled;
	std::list< ItemHVQT > itemHVQTList;
	DWORD source;
	GroupElem group;

public:
	FRL_EXCEPTION_CLASS( InvalidParameter );
	AsyncRequest( GroupElem& group_ );
	AsyncRequest( GroupElem& group_, const std::list< OPCHANDLE > &handles_ );
	AsyncRequest( GroupElem& group_, const std::list< ItemHVQT >& itemsList );
	~AsyncRequest();
	void setTransactionID( DWORD id_ );
	DWORD getTransactionID() const;
	DWORD getCancelID();
	Bool isCancelled();
	void isCancelled( Bool isCancelled_ );
	void init( const std::list< OPCHANDLE > &handles_ );
	void init( const std::list< OPCHANDLE > &handles_, const VARIANT *values_ );
	const std::list< ItemHVQT >& getItemHVQTList() const;
	size_t getCounts() const;
	void removeHandle( OPCHANDLE handle );
	DWORD getSource() const;
	void setSource( DWORD source_ );
	static DWORD getUniqueCancelID();
	GroupElem getGroup();
}; // class AsyncRequest

typedef boost::shared_ptr< AsyncRequest > AsyncRequestListElem;
typedef std::list< AsyncRequestListElem > AsyncRequestList;

} // namespace opc
} // namespace frl

#endif // FRL_PLATFORM == FRL_PLATFORM_WIN32
#endif // frl_opc_async_request_h_
