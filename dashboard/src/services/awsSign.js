import moment from 'moment';
import CryptoJS from 'crypto-js';

/**
 * Get aws signed url.
 * @param protocol
 * @param host
 * @param uri
 * @param service
 * @param region
 * @param accessKey
 * @param secretKey
 * @param sessionToken
 * @return {string} signed url
 */
export function getSignedUrl(protocol, host, uri, service, region, accessKey, secretKey, sessionToken) {
    const time = moment().utc();
    const dateStamp = time.format('YYYYMMDD');
    const amzdate = `${dateStamp}T${time.format('HHmmss')}Z`;
    const algorithm = 'AWS4-HMAC-SHA256';
    const method = 'GET';

    const credentialScope = `${dateStamp}/${region}/${service}/aws4_request`;
    let canonicalQuerystring = 'X-Amz-Algorithm=AWS4-HMAC-SHA256';
    canonicalQuerystring += `&X-Amz-Credential=${encodeURIComponent(`${accessKey}/${credentialScope}`)}`;
    canonicalQuerystring += `&X-Amz-Date=${amzdate}`;
    canonicalQuerystring += '&X-Amz-SignedHeaders=host';

    const canonicalHeaders = `host:${host}\n`;
    const payloadHash = sha256('');
    const canonicalRequest = `${method}\n${uri}\n${canonicalQuerystring}\n${canonicalHeaders}\nhost\n${payloadHash}`;


    const stringToSign = `${algorithm}\n${amzdate}\n${credentialScope}\n${sha256(canonicalRequest)}`;
    const signingKey = getSignatureKey(secretKey, dateStamp, region, service);
    const signature = sign(signingKey, stringToSign);

    canonicalQuerystring += `&X-Amz-Signature=${signature}`;
    if (sessionToken) {
        canonicalQuerystring += `&X-Amz-Security-Token=${encodeURIComponent(sessionToken)}`;
    }

    return `${protocol}://${host}${uri}?${canonicalQuerystring}`;
}

/**
 * @param key
 * @param msg
 */
function sign(key, msg) {
    const hash = CryptoJS.HmacSHA256(msg, key);
    return hash.toString(CryptoJS.enc.Hex);
}

/**
 * @param msg
 */
function sha256(msg) {
    const hash = CryptoJS.SHA256(msg);
    return hash.toString(CryptoJS.enc.Hex);
}

/**
 * @param key
 * @param dateStamp
 * @param regionName
 * @param serviceName
 * @return {*}
 */
function getSignatureKey(key, dateStamp, regionName, serviceName) {
    const kDate = CryptoJS.HmacSHA256(dateStamp, `AWS4${key}`);
    const kRegion = CryptoJS.HmacSHA256(regionName, kDate);
    const kService = CryptoJS.HmacSHA256(serviceName, kRegion);
    return CryptoJS.HmacSHA256('aws4_request', kService);
}
