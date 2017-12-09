#include <Foundation/Foundation.h>
#include <CoreServices/CoreServices.h>

char *convert_mimetype(const char *mimetype) {
	char *ret;
	NSString *str = [[NSString alloc] initWithUTF8String:mimetype];
        CFStringRef uti = UTTypeCreatePreferredIdentifierForTag(kUTTagClassMIMEType, (CFStringRef)str, NULL);
        ret =  [(NSString *)uti UTF8String];
        [str release];
        return ret;
}


char *convert_uti(const char *uti) {
	char *ret = NULL;
	NSString *str = (NSString *)UTTypeCopyPreferredTagWithClass((CFStringRef)[[NSString alloc] initWithUTF8String:uti], kUTTagClassMIMEType);
	ret = [str UTF8String];
	[str release];
	return ret;
}
