// compile: tsc --target es2017 xhr-interceptor.ts

export interface InterceptedRequest {
  method: string;
  url: string;
  requestHeaders: Record<string, string>;
  requestBody:
    | string
    | Document
    | Blob
    | BufferSource
    | FormData
    | URLSearchParams
    | undefined;
  statusCode: number;
  responseHeaders: Record<string, string>;
  responseBody: string | undefined;
}

/** Hook request interceptor for `fetch` and `XMLHttpRequest`. */
export function hookRequestInterceptor(
  shouldInterceptUrl: (url: URL, method: string, statusCode: number) => boolean,
  onIntercept: (intercept: InterceptedRequest) => void
) {
  const defaultFetch = window.fetch;
  const defaultXHR = window.XMLHttpRequest;

  /** Parse headers from a `fetch`ed Headers object. */
  function parseHeaders(headers: Headers | undefined): Record<string, string> {
    if (!headers) return {};
    const obj = {};
    for (const [k, v] of headers.entries()) obj[k] = v;
    return obj;
  }

  /** Literally *parse* headers from an `XMLHttpRequest` header string. */
  function parseRawHeaders(rawHeaders: string): Record<string, string> {
    const lines = rawHeaders.split("\r\n");
    const obj = {};
    for (const line of lines) {
      const parts = line.split(": ");
      if (parts.length < 2) continue;
      const k = parts.shift()!;
      const v = parts.join(": ");
      obj[k] = v;
    }
    return obj;
  }

  /** Wrapping `fetch`. */
  async function hookedFetch(
    ...options: Parameters<typeof fetch>
  ): Promise<Response> {
    const request =
      options[0] instanceof Request ? options[0] : new Request(...options);
    const requestBodyStream = request.clone().body || undefined;
    const response = await defaultFetch(...options);
    const responseBodyStream = response.clone().body || undefined;

    try {
      const method = request.method;
      const url = new URL(response.url);
      const statusCode = response.status;
      if (shouldInterceptUrl(url, method, statusCode)) {
        const requestBody = requestBodyStream
          ? new TextDecoder().decode(
              (await requestBodyStream.getReader().read()).value
            )
          : undefined;
        const responseBody = responseBodyStream
          ? new TextDecoder().decode(
              (await responseBodyStream.getReader().read()).value
            )
          : undefined;
        const intercept: InterceptedRequest = {
          method: request.method,
          url: response.url,
          requestHeaders: parseHeaders(request.headers),
          requestBody: requestBody,
          statusCode: response.status,
          responseHeaders: parseHeaders(response.headers),
          responseBody: responseBody,
        };
        onIntercept(intercept);
      }
    } catch (err) {
      console.error(err);
    }
    return response;
  }

  function interceptXHR(xhr: HookedXHR) {
    try {
      const method = xhr.__requestMethod;
      const url = new URL(xhr.__requestUrl, window.location.origin);
      const statusCode = xhr.status;
      if (shouldInterceptUrl(url, method, statusCode)) {
        const intercept: InterceptedRequest = {
          method: method,
          url: url.href,
          requestHeaders: xhr.__requestHeaders || {},
          requestBody:
            xhr.__requestBody === null ? undefined : xhr.__requestBody,
          statusCode: statusCode,
          responseHeaders: parseRawHeaders(xhr.getAllResponseHeaders()),
          responseBody: xhr.responseText,
        };
        onIntercept(intercept);
      }
    } catch (err) {
      console.error(err);
    }
  }

  class HookedXHR extends defaultXHR {
    __requestMethod: string;
    __requestUrl: string | URL;
    __requestHeaders: Record<string, string> | undefined;
    __requestBody: any;

    setRequestHeader(name: string, value: string): void {
      super.setRequestHeader(name, value);
      this.__requestHeaders = this.__requestHeaders || {};
      this.__requestHeaders[name] = value;
      return;
    }

    open(method: string, url: string | URL) {
      this.__requestMethod = method;
      this.__requestUrl = url;
      return super.open(method, url);
    }

    send(body?: Document | XMLHttpRequestBodyInit | null): void {
      // anyone using this callback always has this set before sending
      const xhr = this;
      const oldStateChange = xhr.onreadystatechange;
      this.onreadystatechange = (ev) => {
        if (xhr.readyState === 4) interceptXHR(xhr);

        if (oldStateChange) oldStateChange.call(xhr, ev);
      };
      this.__requestBody = body;
      return super.send(body);
    }
  }

  window.fetch = hookedFetch;
  window.XMLHttpRequest = HookedXHR;
}
